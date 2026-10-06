#include "../abstract/Card.hpp"
#include <memory>
#include <stack>

class CardStack {
private:
  std::stack<std::shared_ptr<Card>> stack;

public:
  CardStack() = default;

  const std::stack<std::shared_ptr<Card>> &getStack() const { return stack; }
  int getSize() const { return stack.size(); }

  const std::shared_ptr<Card> drawCard();
  void putCard(std::shared_ptr<Card> card);

  void shuffle();

};
