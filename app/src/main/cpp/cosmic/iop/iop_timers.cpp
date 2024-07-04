#include <bitset>
#include <range/v3/algorithm.hpp>
#include <iop/iop_timers.h>

namespace cosmic::iop {
    void IopTimers::clearTimerCounter(u64 index) {
        ioSched->modifyTimerSet(intEvents[index],
            vm::TimerSet::Pause, std::vector<u64>{0});
    }
    struct IopTimersCct {
        u32 counter, control, target;
    };
    std::array<IopTimersCct, 5> iopTimersArea {{
        {0x1f801100, 0x1f801104, 0x1f801108},
        {0x1f801110, 0x1f801114, 0x1f801118},
        {0x1f801120, 0x1f801124, 0x1f801128},
    }};
    os::vec IopTimers::performTimerAccess(u32 address, u32 value, bool write) {
        u64 iopTimerIndex{};
        os::vec result{};
        ranges::for_each(iopTimersArea, [&](const auto& iotMap) {
            if (address >= iotMap.counter && address <= iotMap.target) {
                if (write) {
                    if (iotMap.counter == address)
                        writeCounter(iopTimerIndex, value);
                    else if (iotMap.control == address)
                        writeCtrl(iopTimerIndex, static_cast<u16>(value));
                    else if (iotMap.target == address)
                        writeTarget(iopTimerIndex, value);
                }
            }
            iopTimerIndex++;
        });

        return result;
    }

    IopTimers::IopTimers(std::shared_ptr<vm::Scheduler> &source,
        std::shared_ptr<console::IntCInfra> &infra) :
            ioSched(source), infra(infra) {
    }
    void IopTimers::resetIoTimers() {
        ranges::for_each(ioTimers,
            [](auto& timer){
            timer = {};
        });
        timerIntEnbId = ioSched->createSchedTick(
            false, [&](i32 index, bool overflow){
                timerIrqTest(static_cast<u64>(index), overflow);
        });

        u64 mask{0xffff};
        for (u64 index{0}; index < 6; index++) {
            if (index == 3)
                mask = 0xffffffff;
            auto result{ioSched->placeTickedTask(
                timerIntEnbId, mask, {}, false)};
            intEvents[index] = result.value();
            clearTimerCounter(index);
        }
    }
    void IopTimers::timerIrqTest(u64 index, bool overflow) {
        const auto irqIndex{index};
        auto& irqTimerCtrl{ioTimers[irqIndex].control};
        if (!overflow) {
            if (irqTimerCtrl.zeroReturn) {
                ioTimers[irqIndex].counter = {};
                clearTimerCounter(irqIndex);
            }
        }
        // The IOP will obviously generate numerous interruptions to the EE
        [[likely]] if (irqTimerCtrl.intEnable) {
            const static auto ioIrq{std::array{4, 5, 6, 14, 15, 16}};
            infra->trapIrq(console::EeInt,
                static_cast<u8>(ioIrq[irqIndex]));

            if (overflow)
                irqTimerCtrl.overInterrupt = true;
            else
                irqTimerCtrl.cmpInterrupt = true;
        }

        // Check whether the interrupt signal should repeat
        if (!irqTimerCtrl.repeatInt)
            irqTimerCtrl.intEnable = !irqTimerCtrl.repeatInt;
        else if (irqTimerCtrl.toggleInt)
            irqTimerCtrl.intEnable ^= true;
    }

    u16 IopTimers::readCtrl(u64 index) {
        auto& ctrl{ioTimers[index].control};

        std::bitset<16> ctrlReg{};
        ctrlReg[0] = ctrl.useGate;
        ctrlReg[1] = ctrl.gateMode;

        ctrlReg[3] = ctrl.zeroReturn;
        ctrlReg[4] = ctrl.cmpIntEnb;
        ctrlReg[5] = ctrl.overIntEnb;
        ctrlReg[6] = ctrl.repeatInt;
        ctrlReg[7] = ctrl.toggleInt;
        ctrlReg[8] = ctrl.externSignal;

        ctrlReg[10] = ctrl.intEnable;
        ctrlReg[11] = ctrl.cmpInterrupt;
        ctrlReg[12] = ctrl.overInterrupt;

        ctrlReg[index < 4 ? 9 : 13] = ctrl.preScale;

        ctrl.cmpInterrupt = {};
        ctrl.overInterrupt = {};

        return static_cast<u16>(ctrlReg.to_ulong());
    }
    void IopTimers::writeCounter(u64 index, u32 value) {
        ioTimers[index].counter = value;
        ioSched->modifyTimerSet(intEvents[index], vm::TimerSet::Counter, std::vector<u64>{value});
    }
    void IopTimers::writeCtrl(u64 index, u16 value) {
        auto& timer{ioTimers[index]};
        auto& control{timer.control};
        control.useGate = value & 1;
        if (control.useGate) {

        }
        control.started = !control.useGate;
        control.gateMode = (value >> 1) & 3;
        control.zeroReturn = value & (1 << 3);
        control.cmpIntEnb = value & (1 << 4);
        control.overIntEnb = value & (1 << 5);
        control.repeatInt = value & (1 << 6);
        control.toggleInt = value & (1 << 7);
        control.externSignal = value & (1 << 8);
        control.intEnable = true;

        control.preScale = index < 4 ? (value >> 9) & 0x1 : (value >> 13) & 0x3;

        u32 clockRate;
        enum ClockRateResolution {
            PixelClock,
            HBlank0,
            HBlank1 = 3,
        };

        if (control.externSignal) {
            switch (index) {
            case PixelClock: // pixel clock (13.5 MHz regardless of screen mode)
                clockRate = 15 * 1000 * 1000;
                break;
            case HBlank0:
            case HBlank1:
                clockRate = vm::iopClockRate / 2350;
                break;
            default:
                clockRate = vm::iopClockRate;
            }
        }
        if (!control.externSignal) {
            clockRate = 0;
        }
        enum PreScalesFactor {
            Normal, // IOP clock
            Scale8,
            Scale16,
            Scale256
        };
        switch (static_cast<PreScalesFactor>(control.preScale)) {
        case Normal:
            break;
        case Scale8:
            clockRate /= 8; break;
        case Scale16:
            clockRate /= 16; break;
        case Scale256:
            clockRate /= 256; break;
        }
        timer.counter = {};

        std::vector<u64> sets{clockRate};
        auto ioAddr{intEvents[index]};

        ioSched->modifyTimerSet(ioAddr, vm::TimerSet::ClockRate, sets);
        sets[0] = {};
        ioSched->modifyTimerSet(ioAddr, vm::TimerSet::Counter, sets);
        sets[0] = !control.started;

        ioSched->modifyTimerSet(ioAddr, vm::TimerSet::Pause, sets);
        std::vector<u64> mask{control.overIntEnb, control.cmpIntEnb};
        ioSched->modifyTimerSet(ioAddr, vm::TimerSet::IntMask, mask);
    }

    void IopTimers::writeTarget(u64 index, u32 value) {
        auto& selectedTimer{ioTimers[index]};

        std::vector<u64> target{index};
        selectedTimer.target = target[0];

        ioSched->modifyTimerSet(intEvents[index], vm::TimerSet::Target, target);
        if (!selectedTimer.control.toggleInt)
            selectedTimer.control.intEnable = true;
    }
}

