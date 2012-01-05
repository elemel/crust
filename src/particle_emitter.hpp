#ifndef CRUST_PARTICLE_EMITTER_HPP
#define CRUST_PARTICLE_EMITTER_HPP

namespace crust {
    class Game;

    class ParticleEmitter {
    public:
        explicit ParticleEmitter(Game *game);

        void setPosition(float x, float y);

        void setEnabled(bool enabled);
        void setTargetPosition(float x, float y);
        
        void step(float dt);

        void draw();
        
    private:
        Game *game_;
        float x_;
        float y_;
        bool enabled_;
        float targetX_;
        float targetY_;
        float nextEmitTime_;
        float emitInterval_;
        float minTargetDistance_;
        float particleVelocity_;

        void emit();

        void drawPosition();
        void drawTargetPosition();
    };
}

#endif
