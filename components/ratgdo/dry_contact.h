#pragma once

#ifdef PROTOCOL_DRYCONTACT

#include "esphome/core/defines.h"

#include "esphome/core/gpio.h"
#include "esphome/core/optional.h"

#include "callbacks.h"
#include "observable.h"
#include "protocol.h"
#include "ratgdo_state.h"

namespace esphome {

class Scheduler;
class InternalGPIOPin;

} // namespace esphome

namespace esphome::ratgdo {
namespace dry_contact {

    using namespace esphome::ratgdo::protocol;
    using namespace esphome::gpio;

    class DryContact : public Protocol {
    public:
        void setup(RATGDOComponent* ratgdo, Scheduler* scheduler,
            InternalGPIOPin* rx_pin, InternalGPIOPin* tx_pin);
        void loop();
        void dump_config();

        void sync();

        void light_action(LightAction action);
        void lock_action(LockAction action);
        void door_action(DoorAction action);
        void set_open_limit(bool state);
        void set_close_limit(bool state);
        void send_door_state();

        void set_discrete_open_pin(InternalGPIOPin* pin)
        {
            this->discrete_open_pin_ = pin;
            // Preload the inactive level before enabling output mode. This
            // avoids briefly driving the relay from a retained output latch
            // during ESP8266 startup.
            this->discrete_open_pin_->digital_write(false);
            this->discrete_open_pin_->setup();
            this->discrete_open_pin_->pin_mode(gpio::FLAG_OUTPUT);
            this->discrete_open_pin_->digital_write(false);
        }

        void set_discrete_close_pin(InternalGPIOPin* pin)
        {
            this->discrete_close_pin_ = pin;
            // GPIO16 (D0 on ESP8266) can retain its output latch across a
            // software reset, so force the inactive level before output mode.
            this->discrete_close_pin_->digital_write(false);
            this->discrete_close_pin_->setup();
            this->discrete_close_pin_->pin_mode(gpio::FLAG_OUTPUT);
            this->discrete_close_pin_->digital_write(false);
        }

        Result call(Args args);

        const Traits& traits() const { return this->traits_; }

    protected:
        // Pointers first (4-byte aligned)
        InternalGPIOPin* tx_pin_;
        InternalGPIOPin* rx_pin_;
        InternalGPIOPin* discrete_open_pin_ { nullptr };
        InternalGPIOPin* discrete_close_pin_ { nullptr };
        RATGDOComponent* ratgdo_;
        Scheduler* scheduler_;

        // Traits (likely aligned structure)
        Traits traits_;

        // Small members grouped at the end
        DoorState door_state_;
        struct {
            uint8_t open_limit_reached : 1;
            uint8_t last_open_limit : 1;
            uint8_t close_limit_reached : 1;
            uint8_t last_close_limit : 1;
            uint8_t reserved : 4; // Reserved for future use
        } limits_;
    };

} // namespace dry_contact
} // namespace esphome::ratgdo

#endif // PROTOCOL_DRYCONTACT
