#pragma once

#include "EconomyConstants.hpp"
#include "CorePrices.hpp"
#include "Wave.hpp"

class Economy {
 public:
  Economy() : credits_(STARTING_CREDITS), lives_(STARTING_LIVES) {}

  /**
   * @brief Retrieves the current amount of available credits.
   * @return Total credit balance.
   */
  int getCredits() const {
    return credits_;
  }

  /**
   * @brief Retrieves the current remaining base lives.
   * @return Number of lives left.
   */
  int getLives() const {
    return lives_;
  }

  /**
   * @brief Checks if the player has enough credits to cover a specified price.
   * @param price Cost in credits of the item or structure to check.
   * @return true if current credits are greater than or equal to the price; 
   * false otherwise.
   */
  bool canAfford(int price) const {
    return credits_ >= price;
  }

  /**
   * @brief Buys a core structure if the player can afford it.
   * @param type Structure type to buy.
   * @return true if purchase was successful and credits were deducted; false if credits were insufficient.
   */
  bool buyCore(CoreType type) {
    int price = corePrice(type);

    if (!canAfford(price)) {
      return false;
    }

    credits_ -= price;
    return true;
  }

  /**
   * @brief Adds a reward in credits when an enemy is destroyed.
   * @param amount Bounty credits awarded for the killed enemy.
   */
  void rewardKill(int amount) {
    credits_ += amount;
  }

  /**
   * @brief Applies percentage-based interest to unspent credits at the end of 
   * a round.
   */
  void applyRoundInterest() {
    credits_ += (credits_ * INTEREST_PERCENT) / 100;
  }

  /**
   * @brief Deducts lives when an enemy reaches the base exit.
   * @note Clamps remaining lives to zero if they drop below zero.
   */
  void registerEnemyEscaped() {
    lives_ -= LEAK_PENALTY;
    if (lives_ < 0) {
      lives_ = 0;
    }
  }

  /**
   * @brief Checks if the player has lost the match.
   * @return true if base lives drop to zero or below; false otherwise.
   */
  bool isDefeated() const {
    return lives_ <= 0;
  }

  /**
   * @brief Checks if the player has satisfied the victory condition.
   * @param wavesCompleted Total number of successfully completed waves.
   * @return true if completed waves reach or exceed total required waves; 
   * false otherwise.
   */
  bool isVictory(int wavesCompleted) const {
    return wavesCompleted >= TOTAL_WAVES;
  }

  /**
   * @brief Determines whether the match has ended via victory or defeat.
   * @param wavesCompleted Total number of successfully completed waves.
   * @return true if the game is over; false if execution should continue.
   */
  bool isMatchOver(int wavesCompleted) const {
    return isDefeated() || isVictory(wavesCompleted);
  }

 private:
  int credits_;
  int lives_;
};