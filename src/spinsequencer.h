#ifndef SPIN_SEQ_CLASS_H
#define SPIN_SEQ_CLASS_H
#include <closedloopcontroller.h>
#include <controller.h>
#include <jseq.h>
#include <CanControllerService.h>
#include <RemoteService.h>
#include <JsonTimerService.h>
#include <ESPNowService.h>
class SpinSequencerController
{
public:
    MotorController &motorController;
    ClosedLoopController &controller;
    JsonSeq jsonSeq;
    CanIsoTPController<CanIsoTPMessage> &canController;
    PsychicHttpServer *server;
    RemotesController remotesController = {};
    JsonTimer jsonTimer = {jsonSeq};

    SpinSequencerController(MotorController &motorController, ClosedLoopController &controller, CanIsoTPController<CanIsoTPMessage> &canController, PsychicHttpServer *server) :
            motorController(motorController), controller(controller), jsonSeq(motorController), canController(canController), server(server) 
    {
        canController.messageCallback = [this](String message) {
            jsonSeq.readCommand(message);
        };
        canController.clientMappingCallback = [this](uint32_t rxId) {
            remotesController.addRemote(rxId);
        };
        jsonSeq.broadcastMessage = [&](String message) {
            canController.sendMessage(message, uint32_t(0));
        };
        jsonSeq.sendMessage = [&](String message, uint32_t address) {
            canController.sendMessage(message, address);
        };
    }
    
    void init() 
    {
        jsonTimer.begin();
    }

    void run()
    {
        controller.run();
        motorController.tick();
        jsonSeq.tick();
    }
};
#endif