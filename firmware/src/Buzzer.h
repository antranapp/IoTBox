#include "Particle.h"

class Buzzer {

    public:
        Buzzer(uint8_t buzzerPin);

        void startForStart();
        void startForOpenOperation();
        void startForOpen();
        void startForClose();
        void startForConfiguration();
        void startForReminder();

        static const int melodyStart[3];
        static const int noteDurationsStart[3];

        static const int melodyOpen[3];
        static const int noteDurationsOpen[3];

        static const int melodyClose[3];
        static const int noteDurationsClose[3];

        static const int melodyConfiguration[3];
        static const int noteDurationsConfiguration[3];

        static const int melodyReminder[3];
        static const int noteDurationsReminder[3];

    private:
        uint8_t _buzzerPin;

        void makeSound(const int *melody, const int *duration);
};
