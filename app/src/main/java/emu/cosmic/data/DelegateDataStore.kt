package emu.cosmic.data

import androidx.datastore.preferences.core.edit
import kotlin.properties.ReadWriteProperty
import kotlin.reflect.KProperty
import androidx.annotation.WorkerThread
import emu.cosmic.dataSettings
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.runBlocking

class DelegateDataStore<T>(
    card: SettingContainer<T>,
) : ReadWriteProperty<Any, T> {
    private val context = card.containerContext
    private val globalStorage = context.dataSettings
    private val key = card.preferKey
    private val defaultValue = card.defaultValue

    @WorkerThread
    override operator fun getValue(thisRef: Any, property: KProperty<*>): T {
        var value = defaultValue
        runBlocking(Dispatchers.IO) {
            val dsValueFlow: Flow<T> = globalStorage.data.map { dsReadable ->
                dsReadable[key] ?: defaultValue!!
            }
            value = dsValueFlow.first()
        }
        return value!!
    }
    override operator fun setValue(thisRef: Any, property: KProperty<*>, value: T) {
        val savedValue: T = value ?: defaultValue
        var lastValue: T? = defaultValue
        runBlocking {
            globalStorage.edit { dsEditor ->
                lastValue = dsEditor[key]
                dsEditor[key] = savedValue
            }
        }
        if (savedValue != lastValue) {
            CosmicSettings.updateSettings = true
        }
    }
}