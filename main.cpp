#include <set>
#include <string>
#include <vector>

using std::string;

enum class ToolType {
    Proxy,
    Mediator,
    Observer
};

class BInterface {
public:
    virtual string getState() = 0;
    virtual void setState(string) = 0;
};

class Subscriber {
public:
    virtual void notify() = 0;
};

class BSubscribableInterface : public BInterface {
public:
    virtual void subscribe(Subscriber*) = 0;
    virtual void unsubscribe(Subscriber*) = 0;
};

class B : public BInterface {
public:
    virtual string getState() {
        printf("getState %d %s\n", id, state.c_str());
        return state;
    }
    virtual void setState(string newState) {
        printf("setState %d %s instead %s\n", id, newState.c_str(), state.c_str());
        state = newState;
    }
    B(int id, string state) : id(id), state(state) {}
private:
    string state;
    int id;
};

class A : public Subscriber {
public:
    virtual void notify() {
        printf("\nA notified!!!!!\n\n");
    }
    explicit A(BInterface* b) : b(b) {
        subscribable = false;
    }
    explicit A(BSubscribableInterface* b) : b(b) {
        subscribable = true;
    }

    void subscribe() {
        if (subscribable)
            ((BSubscribableInterface*)b)->subscribe(this);
        else
            printf("not implemented\n");
    }
    void unsubscribe() {
        if (subscribable)
            ((BSubscribableInterface*)b)->unsubscribe(this);
        else
            printf("not implemented\n");
    }
    string checkBState() {
        return b->getState();
    }
    void changeBState(string newState) {
        b->setState(newState);
    }
private:
    BInterface* b;
    bool subscribable;
};

template <ToolType T> class SwissKnife {};

template<>
class SwissKnife<ToolType::Proxy> : public BInterface {
public:
    SwissKnife(B* b) : b(b) {}

    virtual string getState() {
        printf("proxing get\n");
        return b->getState();
    }
    virtual void setState(string newState) {
        printf("proxing set\n");
        b->setState(newState);
    }
private:
    B* b;
};

template<>
class SwissKnife<ToolType::Mediator> : public BInterface {
public:
    SwissKnife(B* b_) {
        b.push_back(b_);
    }

    void addB(B* b_) {
        b.push_back(b_);
    }

    virtual string getState() {
        printf("mediate get\n");
        return b[getId()]->getState();
    }
    virtual void setState(string newState) {
        printf("mediate set\n");
        b[getId()]->setState(newState);
    }
private:
    int getId() {
        return rand() % b.size();
    }
    std::vector<B*> b;
};


template<>
class SwissKnife<ToolType::Observer> : public BSubscribableInterface {
public:
    SwissKnife(B* b) : b(b) {}

    virtual string getState() {
        printf("observe get\n");
        return b->getState();
    }
    virtual void setState(string newState) {
        printf("observe set\n");
        
        b->setState(newState);
        
        for (auto subscriber : subscribers) {
            subscriber->notify();
        }
    }
    virtual void subscribe(Subscriber* subscriber) {
        subscribers.insert(subscriber);
    }
    virtual void unsubscribe(Subscriber* subscriber) {
        subscribers.erase(subscriber);
    }
private:
    B* b;
    std::set<Subscriber*> subscribers;
};

int main() {
    {
        // proxy

        printf("\n-------------\nproxy:\n-------------\n");
        
        B* b = new B(0, "b");
        SwissKnife<ToolType::Proxy>* b_proxy = new SwissKnife<ToolType::Proxy>(b);
        A a((BInterface*)b_proxy);
        a.changeBState("???");
        printf("state: %s\n", a.checkBState().c_str());
    }

    {
        // mediator
        
        printf("\n-------------\nmediator:\n-------------\n");

        srand(1);
        B* b = new B(0, "b");
        SwissKnife<ToolType::Mediator>* b_mediator = new SwissKnife<ToolType::Mediator>(b);
        B* b1 = new B(1, "b1");
        b_mediator->addB(b1);

        A a((BInterface*)b_mediator);

        a.changeBState("???");
        printf("state 1: %s\n", a.checkBState().c_str());
        a.changeBState("!!!");
        printf("state 2: %s\n", a.checkBState().c_str());
    }

    {
        // observer
        
        printf("\n-------------\nobserver:\n-------------\n");

        B* b = new B(0, "b");
        SwissKnife<ToolType::Observer>* b_mediator = new SwissKnife<ToolType::Observer>(b);

        A a((BSubscribableInterface*)b_mediator);

        a.subscribe();
        a.changeBState("???");
        printf("state: %s\n", a.checkBState().c_str());


        a.unsubscribe();
        a.changeBState("!!!");
        printf("state: %s\n", a.checkBState().c_str());
    }

    // да, утекло много воды (памяти)
}
