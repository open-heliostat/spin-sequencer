// #include "CanControllerService.h"

// std::pair<String, JsonObject> traverseJsonPath(JsonObject obj) {
//     String path;
//     if (obj.begin()->value().is<JsonObject>()) {
//         JsonObject child = obj.begin()->value().as<JsonObject>();
//         path += obj.begin()->key().c_str();
//         path += "/";
//         // make path from nested keys
//         while (child.begin()->value().is<JsonObject>() && child.size() == 1) {
//             path += child.begin()->key().c_str();
//             path += "/";
//             child = child.begin()->value().as<JsonObject>();
//         }
//         ESP_LOGI("CAN", "Path: %s", path.c_str());
//         return std::make_pair(path, child);
//     }
//     return std::make_pair(path, obj);
// }

// JsonRouter<CanIsoTPController<CanIsoTPMessage>> CanControllerJsonRouter::router = JsonRouter<CanIsoTPController<CanIsoTPMessage>>(
// {
//     {"sendMessage", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
//         CanIsoTPMessage message;
//         if (value.is<JsonObject>() && value["message"].is<String>()) {
//             strcpy(message.message, value["message"].as<String>().c_str());
//             // make sure the message is null terminated
//             message.message[sizeof(message.message) - 1] = '\0';
//             if (value["txId"].is<uint32_t>()) {
//                 if (controller.sendMessage(message, value["txId"].as<uint32_t>())) value.as<JsonObject>()["error"] = "OK";
//                 else value.as<JsonObject>()["error"] = "FAILED";
//             }
//             else if (controller.sendMessage(message)) value.as<JsonObject>()["error"] = "OK";
//             else value.as<JsonObject>()["error"] = "FAILED";
//         }
//         else if (value.is<String>()) {
//             strcpy(message.message, value.as<String>().c_str());
//             // make sure the message is null terminated
//             message.message[sizeof(message.message) - 1] = '\0';
//             if (controller.sendMessage(message)) value.as<JsonObject>()["error"] = "OK";
//             else value.as<JsonObject>()["error"] = "FAILED";
//         }
//         else {
//             value.as<JsonObject>()["error"] = "INVALID MESSAGE";
//         }
//         return false;
//     }},
//     {"txId", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
//         if (value.is<uint32_t>()) {
//             controller.setTxId(value.as<uint32_t>());
//             return true;
//         }
//         return false;
//     }},
//     {"rxId", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
//         if (value.is<uint32_t>()) {
//             controller.setRxId(value.as<uint32_t>());
//             return true;
//         }
//         return false;
//     }},
//     {"enabled", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
//         if (value.is<bool>()) {
//             controller.enabled = value.as<bool>();
//             return true;
//         }
//         return false;
//     }},
//     {"speed", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
//         if (value.is<long>()) {
//             // controller.setSpeed(value.as<long>());
//             return true;
//         }
//         return false;
//     }},
//     {"tun", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
//         if (value.is<JsonObject>()) {
//             JsonObject obj = value.as<JsonObject>();
//             // get first child of the object
//             String id = obj.begin()->key().c_str();
//             ESP_LOGI("CAN", "ID: %s", id.c_str());
//             if (id.toInt() > 0) {
//                 // convert key to integer
//                 int index = id.toInt();
//                 auto req = traverseJsonPath(obj[id]);
//                 // ESP_LOGI("CAN", "Path: %s", req.first.c_str());
//                 String response = controller.jsonPOST(req.first, index, req.second);
//                 if (deserializeJson(JsonVariant(req.second), response) != DeserializationError::Ok) {
//                     ESP_LOGI("CAN", "Failed to deserialize JSON: %s", response.c_str());
//                     value["error"] = "Failed to deserialize JSON";
//                 }
//             }
//         }
//         return false;
//     }},
//     {"messagePack", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
//         if (value.is<bool>()) {
//             controller.messagePack = value.as<bool>();
//             return true;
//         }
//         return false;
//     }},
//     {"mapClients", [](JsonVariant value, CanIsoTPController<CanIsoTPMessage>& controller) {
//         controller.mapClients();
//         return false;
//     }}
// },
// {
//     {"messageHistory", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
//         JsonArray messages = content.to<JsonArray>();
//         // create array from message history vector
//         int size = controller.getMessageHistory().size();
//         if (size > 0) {
//             String msgs[size];
//             for (int i = 0; i < size; i++) {
//                 msgs[i] = controller.getMessageHistory()[i];
//             }
//             copyArray(msgs, size, messages);
//         }
//     }},
//     {"txId", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
//         content.set(controller.txId);
//     }},
//     {"rxId", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
//         content.set(controller.rxId);
//     }},
//     {"enabled", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
//         content.set(controller.enabled);
//     }},
//     {"speed", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
//         content.set(controller.getSpeed());
//     }},
//     {"tun", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
//         if (content.is<JsonObject>()) {
//             JsonObject obj = content.as<JsonObject>();
//             // get first child of the object
//             String id = obj.begin()->key().c_str();
//             ESP_LOGI("CAN", "ID: %s", id.c_str());
//             if (id.toInt() > 0) {
//                 // convert key to integer
//                 int index = id.toInt();
//                 auto req = traverseJsonPath(obj[id]);
//                 // ESP_LOGI("CAN", "Path: %s", req.first.c_str());
//                 String response = controller.jsonGET(req.first, index, req.second);
//                 if (deserializeJson(JsonVariant(req.second), response) != DeserializationError::Ok) {
//                     ESP_LOGI("CAN", "Failed to deserialize JSON: %s", response.c_str());
//                     content["error"] = "Failed to deserialize JSON";
//                 }
//                 // ESP_LOGI("CAN", "Response: %s", content.as<String>().c_str());
//                 // content.set(response);
//             }
//         }
//     }},
//     {"messagePack", [](CanIsoTPController<CanIsoTPMessage>& controller, JsonVariant content) {
//         content.set(controller.messagePack);
//     }},
// });

// void CanControllerService::begin()
// {
//     _httpRouterEndpoint.begin();
//     _fsPersistence.readFromFS();
//     _state.begin();
// }

// void CanControllerService::loop()
// {
//     _state.loop();
// }