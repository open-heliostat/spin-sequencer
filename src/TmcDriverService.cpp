#include <TmcDriverService.h>

JsonRouter<TMC5160Stepper> TMC5160StepperJsonRouter::router = JsonRouter<TMC5160Stepper>(
{
    {"chopConfig", [](JsonVariant content, TMC5160Stepper &driver) {
        return chopConfigRouter.parse(content, driver);
    }},
    {"stallConfig", [](JsonVariant content, TMC5160Stepper &driver) {
        return stallConfigRouter.parse(content, driver);
    }},
    {"pwmConfig", [](JsonVariant content, TMC5160Stepper &driver) {
        return pwmConfigRouter.parse(content, driver);
    }}
},
{
    {"status", [](TMC5160Stepper &driver, const JsonVariant target) {
        target["enabled"] = !driver.drv_enn();
        target["stallguard"] = driver.stallguard();
        target["overtemp"] = driver.ot();
        target["overtemp_warning"] = driver.otpw();
        target["cs_actual"] = driver.cs_actual();
        target["current_scale"] = driver.PWM_SCALE();
    }},
    {"chopConfig", [](TMC5160Stepper &driver, const JsonVariant target) {
        target["toff"] = driver.toff();
        target["hstrt"] = driver.hstrt();
        target["hend"] = driver.hend();
        target["tbl"] = driver.tbl();
        // target["vsense"] = driver.vsense();
        target["mres"] = driver.mres();
    }},
    {"pwmConfig", [](TMC5160Stepper &driver, const JsonVariant target) {
        target["pwm_ofs"] = driver.pwm_ofs();
        target["pwm_grad"] = driver.pwm_grad();
        target["pwm_freq"] = driver.pwm_freq();
        target["pwm_autoscale"] = driver.pwm_autoscale();
        target["pwm_autograd"] = driver.pwm_autograd();
    }},
    {"stallConfig", [](TMC5160Stepper &driver, const JsonVariant target) {
        target["sg_stop"] = driver.sg_stop();
        target["semin"] = driver.semin();
        target["semax"] = driver.semax();
        target["sedn"] = driver.sedn();
        target["sgt"] = driver.sgt();
    }},
});

JsonEventRouter<TMC5160Stepper> TMC5160StepperJsonRouter::chopConfigRouter = JsonEventRouter<TMC5160Stepper>({
    {"toff", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.toff(content.as<int>());
            return true;
        }
        return false;
    }},
    {"hstrt", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.hstrt(content.as<int>());
            return true;
        }
        return false;
    }},
    {"hend", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.hend(content.as<int>());
            return true;
        }
        return false;
    }},
    {"tbl", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.tbl(content.as<int>());
            return true;
        }
        return false;
    }},
    // {"vsense", [](JsonVariant content, TMC5160Stepper &driver) {
    //     if (content.is<bool>()) {
    //         driver.vsense(content.as<bool>());
    //         return true;
    //     }
    //     return false;
    // }},
    {"mres", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.mres(content.as<int>());
            return true;
        }
        return false;
    }}
});

JsonEventRouter<TMC5160Stepper> TMC5160StepperJsonRouter::stallConfigRouter = JsonEventRouter<TMC5160Stepper>({
    {"sg_stop", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<bool>()) {
            driver.sg_stop(content.as<bool>());
            return true;
        }
        return false;
    }},
    {"semin", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.semin(content.as<int>());
            return true;
        }
        return false;
    }},
    {"semax", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.semax(content.as<int>());
            return true;
        }
        return false;
    }},
    {"sedn", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.sedn(content.as<int>());
            return true;
        }
        return false;
    }},
    {"sgt", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.sgt(content.as<int>());
            return true;
        }
        return false;
    }}
});

JsonEventRouter<TMC5160Stepper> TMC5160StepperJsonRouter::pwmConfigRouter = JsonEventRouter<TMC5160Stepper>({
    {"pwm_ofs", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.pwm_ofs(content.as<int>());
            return true;
        }
        return false;
    }},
    {"pwm_grad", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.pwm_grad(content.as<int>());
            return true;
        }
        return false;
    }},
    {"pwm_freq", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<int>()) {
            driver.pwm_freq(content.as<int>());
            return true;
        }
        return false;
    }},
    {"pwm_autoscale", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<bool>()) {
            driver.pwm_autoscale(content.as<bool>());
            return true;
        }
        return false;
    }},
    {"pwm_autograd", [](JsonVariant content, TMC5160Stepper &driver) {
        if (content.is<bool>()) {
            driver.pwm_autograd(content.as<bool>());
            return true;
        }
        return false;
    }}
});

