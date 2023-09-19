package emu.zenith.data

import android.content.Context
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.edit
import kotlin.properties.ReadWriteProperty
import kotlin.reflect.KProperty
import androidx.annotation.WorkerThread
import emu.zenith.dataSettings
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.runBlocking

class DelegateDataStore<T>(
    context: Context,
    private val key: Preferences.Key<T>,
    private val defaultValue: T
) : ReadWriteProperty<Any, T> {
    private val dsDb = context.dataSettings

    @WorkerThread
    override fun getValue(thisRef: Any, property: KProperty<*>): T {
        var value = defaultValue
        runBlocking {
            val dsValueFlow: Flow<T> = dsDb.data.map { dsDb ->
                dsDb[key] ?: defaultValue
            }
            value = dsValueFlow.first()
        }
        return value
    }
    override fun setValue(thisRef: Any, property: KProperty<*>, value: T) {
        suspend {
            dsDb.edit { dsEditor ->
                dsEditor[key] = value ?: defaultValue
            }
        }
    }
}