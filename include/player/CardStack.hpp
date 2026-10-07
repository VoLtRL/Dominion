#include "../abstract/Card.hpp"
#include <memory>
#include <stack>

class CardStack {
private:
  std::stack<const Card*> stack;

public:
  CardStack() = default;

  const std::stack<const Card*> &getStack() const { return stack; }
  int getSize() const { return stack.size(); }

  const Card* drawCard();
  void putCard(const Card* card);

  void shuffle();

};
