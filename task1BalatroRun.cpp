/*
 * ============================================================================
 * TUGAS 1 — BUILDING A BALATRO-LIKE RUN (SINGLE-FILE OOP IMPLEMENTATION)
 * Mata Kuliah: Desain Pattern
 * ============================================================================
 * 
 * REFLECTION (MANDATORY — 200-300 WORDS):
 * (Panjang teks refleksi di bawah: 223 kata)
 * 
 * 1. Apa struktur invariant di dalam program Anda?
 * Struktur invariant dalam program ini adalah alur eksekusi inti (core loop) 
 * yang diatur oleh kelas RunSession. Urutan 6 fase pada setiap round bersifat 
 * mutlak dan tidak boleh diubah: (1) Generate input, (2) Compute base score, 
 * (3) Compute reward, (4) Update money, (5) Shop phase, dan (6) Advance round. 
 * Jumlah total 3 round serta aturan bahwa pembaruan uang hanya terjadi pada 
 * fase reward juga merupakan bagian dari invariant sistem.
 * 
 * 2. Bagian mana saja yang bersifat mutable?
 * Bagian mutable mencakup seluruh algoritma dan perilaku konkret yang didelegasikan 
 * ke antarmuka (interface): cara masukan dihasilkan (IInputGenerator), perhitungan 
 * skor (IScoringRule), konversi skor menjadi uang (IRewardRule), serta penyajian 
 * penawaran barang pada toko (ShopSystem). Implementasi konkret seperti 
 * FixedInputGenerator, RandomInputGenerator, DefaultRewardRule, maupun BonusRewardRule 
 * dapat diganti secara dinamis tanpa mengganggu controller.
 * 
 * 3. Ketika Anda mengganti InputGenerator, mengapa RunSession tidak perlu diubah?
 * RunSession menerapkan prinsip Dependency Inversion dan Polymorphism. Kelas 
 * tersebut hanya bergantung pada abstraksi murni IInputGenerator, bukan pada kelas 
 * konkret. Ketika FixedInputGenerator diganti menjadi RandomInputGenerator, kontrak 
 * method generateInput() tetap identik. RunSession menerima objek melalui constructor 
 * injection, sehingga perubahan perilaku masukan terjadi di luar kontroler tanpa 
 * memerlukan modifikasi kode internal sedikit pun (Open/Closed Principle).
 * 
 * 4. Apa yang akan terjadi jika logika scoring dimasukkan langsung ke dalam RunSession?
 * Jika perhitungan skor dimasukkan langsung ke dalam RunSession, terjadi pelanggaran 
 * prinsip Single Responsibility Principle (SRP) dan tercipta kopling ketat (tight coupling). 
 * Controller kehilangan sifat invariant karena setiap variasi aturan skor (misalnya efek 
 * kartu baru atau multiplier) akan memaksa modifikasi pada RunSession. Hal ini meningkatkan 
 * risiko regresi, merusak integritas alur utama, dan mempersulit pengujian unit (unit testing).
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

// ============================================================================
// 1. DATA MODEL
// ============================================================================
struct TurnInput {
    string handName;
    int rawValue;
};

// ============================================================================
// 2. ABSTRACT INTERFACES (MUTABLE ABSTRACTIONS)
// ============================================================================

// Interface for generating turn inputs
class IInputGenerator {
public:
    virtual ~IInputGenerator() = default;
    virtual TurnInput generateInput(int roundNumber) = 0;
};

// Interface for computing base score from input
class IScoringRule {
public:
    virtual ~IScoringRule() = default;
    virtual int computeScore(const TurnInput& input) = 0;
};

// Interface for computing reward money from base score
class IRewardRule {
public:
    virtual ~IRewardRule() = default;
    virtual int computeReward(int baseScore, int roundNumber) = 0;
};

// ============================================================================
// 3. SHOP SYSTEM MODULE
// ============================================================================
class ShopSystem {
public:
    virtual ~ShopSystem() = default;

    virtual void displayOffer(int roundNumber) {
        if (roundNumber == 1) {
            cout << "[SHOP] offered: Bonus(+2) cost 2 | status: skipped\n";
        } else if (roundNumber == 2) {
            cout << "[SHOP] offered: Foil Card cost 3 | status: skipped\n";
        } else {
            cout << "[SHOP] offered: Joker Voucher cost 5 | status: skipped\n";
        }
    }
};

// ============================================================================
// 4. BASE CONCRETE IMPLEMENTATIONS
// ============================================================================

// Base Input Generator: Produces deterministic / fixed input
class FixedInputGenerator : public IInputGenerator {
public:
    TurnInput generateInput(int roundNumber) override {
        TurnInput input;
        input.handName = "Hand #" + to_string(roundNumber);
        input.rawValue = 10 + (roundNumber * 5); // 15, 20, 25
        return input;
    }
};

// Base Scoring Rule: Computes score directly from rawValue
class DefaultScoringRule : public IScoringRule {
public:
    int computeScore(const TurnInput& input) override {
        return input.rawValue;
    }
};

// Base Reward Rule: 1-to-1 conversion between base score and money
class DefaultRewardRule : public IRewardRule {
public:
    int computeReward(int baseScore, [[maybe_unused]] int roundNumber) override {
        return baseScore;
    }
};

// ============================================================================
// 5. REQUIRED MODIFICATIONS (PROVING ARCHITECTURE DISCIPLINE)
// ============================================================================

// Modification 1: Dynamic / Randomized input generation
class RandomInputGenerator : public IInputGenerator {
private:
    mt19937 rng;
    vector<string> possibleHands;

public:
    RandomInputGenerator() {
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        rng.seed(seed);
        possibleHands = {
            "High Card (Rank: Jack)",
            "Pair of Aces",
            "Two Pair (Kings & Tens)",
            "Three of a Kind (Queens)",
            "Flush (Spades)",
            "Full House (Aces full of Kings)"
        };
    }

    TurnInput generateInput([[maybe_unused]] int roundNumber) override {
        uniform_int_distribution<size_t> handDist(0, possibleHands.size() - 1);
        uniform_int_distribution<int> valDist(15, 35);

        TurnInput input;
        input.handName = possibleHands[handDist(rng)];
        input.rawValue = valDist(rng);
        return input;
    }
};

// Modification 2: Changed reward logic (reward is NOT equal to base score)
class BonusRewardRule : public IRewardRule {
public:
    int computeReward(int baseScore, int roundNumber) override {
        // Flat bonus (+2) and doubles reward on even rounds
        if (roundNumber % 2 == 0) {
            return (baseScore + 2) * 2;
        }
        return baseScore + 2;
    }
};

// ============================================================================
// 6. INVARIANT CONTROLLER (RUN SESSION)
// STRICT CONSTRAINT: THIS CLASS MUST NEVER CHANGE WHEN BEHAVIORS ARE MODIFIED
// ============================================================================
class RunSession {
private:
    IInputGenerator* inputGenerator;
    IScoringRule* scoringRule;
    IRewardRule* rewardRule;
    ShopSystem* shopSystem;

    int money;
    int currentRound;
    static constexpr int TOTAL_ROUNDS = 3;

public:
    // Dependency Injection via constructor
    RunSession(IInputGenerator* inputGen,
               IScoringRule* scoreRule,
               IRewardRule* rewRule,
               ShopSystem* shop)
        : inputGenerator(inputGen),
          scoringRule(scoreRule),
          rewardRule(rewRule),
          shopSystem(shop),
          money(0),
          currentRound(1) {}

    // Executes the core 3-round invariant game loop
    void run() {
        cout << "=== RUN START ===\n\n";

        while (currentRound <= TOTAL_ROUNDS) {
            cout << "--- Round " << currentRound << " ---\n";

            // Phase 1: Generate input (delegated to IInputGenerator)
            TurnInput input = inputGenerator->generateInput(currentRound);
            cout << "[PLAY] input generated: " << input.handName << "\n";

            // Phase 2: Compute base score (delegated to IScoringRule)
            int baseScore = scoringRule->computeScore(input);
            cout << "[SCORE] base score: " << baseScore << "\n";

            // Phase 3: Compute reward (delegated to IRewardRule)
            int gain = rewardRule->computeReward(baseScore, currentRound);

            // Phase 4: Update money (money ONLY updated here in reward phase)
            money += gain;
            cout << "[REWARD] gain: " << gain << " | money: " << money << "\n";

            // Phase 5: Shop phase (delegated to ShopSystem)
            shopSystem->displayOffer(currentRound);

            // Phase 6: Advance round
            currentRound++;
            cout << "\n";
        }

        cout << "=== RUN END ===\n";
        cout << "Final money: " << money << "\n";
    }

    int getMoney() const {
        return money;
    }
};

// ============================================================================
// 7. ENTRY POINT (MAIN)
// ============================================================================
int main() {
    cout << "====================================================\n";
    cout << "     TASK 1: BALATRO-LIKE RUN (SINGLE FILE OOP)     \n";
    cout << "====================================================\n\n";

    // -------------------------------------------------------------
    // 1. BASE SYSTEM EXECUTION
    // Demonstrating standard behavior: FixedInput, DefaultReward
    // -------------------------------------------------------------
    cout << ">>> RUNNING BASE SYSTEM (Fixed Input, Base Reward) <<<\n\n";
    {
        FixedInputGenerator fixedGen;
        DefaultScoringRule defaultScore;
        DefaultRewardRule defaultReward;
        ShopSystem shop;

        // Injected RunSession
        RunSession baseSession(&fixedGen, &defaultScore, &defaultReward, &shop);
        baseSession.run();
    }

    cout << "\n====================================================\n\n";

    // -------------------------------------------------------------
    // 2. MODIFIED SYSTEM EXECUTION
    // Modification 1: RandomInputGenerator
    // Modification 2: BonusRewardRule
    // CRITICAL: RunSession remains 100% UNCHANGED!
    // -------------------------------------------------------------
    cout << ">>> RUNNING MODIFIED SYSTEM (Modification 1 & 2) <<<\n";
    cout << ">>> (RandomInputGenerator + BonusRewardRule) <<<\n\n";
    {
        RandomInputGenerator randomGen;
        DefaultScoringRule defaultScore;
        BonusRewardRule bonusReward;
        ShopSystem shop;

        // Exact same invariant RunSession class injected with modified components
        RunSession modifiedSession(&randomGen, &defaultScore, &bonusReward, &shop);
        modifiedSession.run();
    }

    cout << "\n====================================================\n";
    cout << "Demonstration completed successfully.\n";
    return 0;
}
