package emu.cosmic.models

import androidx.lifecycle.ViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import java.util.concurrent.atomic.AtomicBoolean

data class EmulationStatus(
    var running: AtomicBoolean = AtomicBoolean(false)
)

class EmulationModel : ViewModel() {
    private val status = MutableStateFlow(EmulationStatus())
    private val liveStatus = status.asStateFlow()
    fun checkRunning(state: Boolean) {
        liveStatus.value.apply {
            running.set(state)
        }
    }
    fun isRunning(): Boolean {
        return liveStatus.value.running.get()
    }
}