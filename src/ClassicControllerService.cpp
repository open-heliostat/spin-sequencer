#include "ClassicControllerService.h"

JsonRouter<MotorController> ClassicControllerJsonRouter::router = JsonRouter<MotorController>(
{
    {"control", [](JsonVariant content, MotorController &controller) {
        return controlRouter.parse(content, controller);
    }}
},
{
    {"control", [](MotorController &controller, const JsonVariant target) {
        target["position"] = controller.getPosition();
        target["isMoving"] = controller.isMoving;
        target["pingPong"] = controller.pingPong;
        target["stopPosition"] = controller.stopPosition;
        target["dist"] = controller.dist;
        target["speed"] = controller.speed;
        target["offset"] = controller.sensor.offset;
    }}
});

JsonEventRouter<MotorController> ClassicControllerJsonRouter::controlRouter = JsonEventRouter<MotorController>({
    {"position", [](JsonVariant content, MotorController &controller) {
        if (content.is<double>()) {
            controller.setPosition(content.as<double>());
            return true;
        }
        return false;
    }},
    {"stop", [](JsonVariant content, MotorController &controller) {
        controller.stop();
        return true;
    }},
    {"pingpong", [](JsonVariant content, MotorController &controller) {
        if (content.is<bool>()) {
            controller.setPingPong(content.as<bool>());
            return true;
        }
        return false;
    }},
    {"stopPosition", [](JsonVariant content, MotorController &controller) {
        if (content.is<double>()) {
            controller.stopPosition = content.as<double>();
            return true;
        }
        return false;
    }},
    {"dist", [](JsonVariant content, MotorController &controller) {
        if (content.is<double>()) {
            controller.dist = content.as<double>();
            return true;
        }
        return false;
    }},
    {"speed", [](JsonVariant content, MotorController &controller) {
        if (content.is<double>()) {
            controller.speed = content.as<double>();
            return true;
        }
        return false;
    }},
    {"offset", [](JsonVariant content, MotorController &controller) {
        if (content.is<double>()) {
            controller.sensor.offset = content.as<double>();
            return true;
        }
        return false;
    }},
});