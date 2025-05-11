#ifndef SPIN_SEQ_CLASS_H
#define SPIN_SEQ_CLASS_H
#include <closedloopcontroller.h>
#include <controller.h>
#include <jseq.h>
#include <CanControllerService.h>
class SpinSequencerController
{
public:
    MotorController &motorController;
    ClosedLoopController &controller;
    JsonSeq jsonSeq;
    CanIsoTPController<CanIsoTPMessage> &canController;
    PsychicHttpServer *server;

    SpinSequencerController(MotorController &motorController, ClosedLoopController &controller, CanIsoTPController<CanIsoTPMessage> &canController, PsychicHttpServer *server) :
        motorController(motorController), controller(controller), jsonSeq(motorController), canController(canController), server(server) {}
    
    void init() 
    {
    }

    void run()
    {
        controller.run();
        motorController.tick();
        jsonSeq.tick();
    }
};
#endif