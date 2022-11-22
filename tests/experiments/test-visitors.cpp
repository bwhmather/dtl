#include "dtl-test.hpp"

namespace dtt {

struct LiteralVisitor;
struct NodeVisitor;

struct Node {
    virtual ~Node(){};
    virtual void
    accept(NodeVisitor& visitor) = 0;
};

struct Literal : public Node {
    void
    accept(NodeVisitor& visitor) override final;
    virtual void
    accept(LiteralVisitor& visitor) = 0;
};

struct Int : public Literal {
    void
    accept(LiteralVisitor& visitor) override final;
};

struct Float : Literal {
    void
    accept(LiteralVisitor& visitor) override final;
};

struct Select : Node {
    void
    accept(NodeVisitor& visitor) override final;
};

struct LiteralVisitor {
    virtual ~LiteralVisitor(){};
    virtual void
    visit(Int& node) = 0;
    virtual void
    visit(Float& node) = 0;
};

struct NodeVisitor : public LiteralVisitor {
    virtual ~NodeVisitor(){};
    virtual void
    visit(Select& node) = 0;
};

void
Literal::accept(NodeVisitor& visitor) {
    this->accept(static_cast<LiteralVisitor&>(visitor));
}

void
Int::accept(LiteralVisitor& visitor) {
    visitor.visit(*this);
}

void
Float::accept(LiteralVisitor& visitor) {
    visitor.visit(*this);
}

void
Select::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

template <typename N>
struct IsVisitor : NodeVisitor {
    bool result;

    void
    visit(Node& node) {
        result = false;
    }

    void
    visit(N& node) {
        result = true;
    }
};

template <typename N>
bool
is<N>(Node& n) {
    IsVisitor<N> visitor;
    n.accept(visitor);
    return visitor.result;
}

} // namespace dtt

int
main(void) {
    dtt::Int int_node;
    dtt::Float float_node;
    dtt::Select select_node;

    dtl_assert(dtt::is<dtt::Int>(static_cast<dtt::Node&>(int_node)));
    dtl_assert(!dtt::is<dtt::Float>(static_cast<dtt::Node&>(int_node)));
    dtl_assert(!dtt::is<dtt::Select>(static_cast<dtt::Node&>(int_node)));

    dtl_assert(!dtt::is<dtt::Int>(static_cast<dtt::Node&>(float_node)));
    dtl_assert(dtt::is<dtt::Float>(static_cast<dtt::Node&>(float_node)));
    dtl_assert(!dtt::is<dtt::Select>(static_cast<dtt::Node&>(float_node)));

    dtl_assert(!dtt::is<dtt::Int>(static_cast<dtt::Node&>(select_node)));
    dtl_assert(!dtt::is<dtt::Float>(static_cast<dtt::Node&>(select_node)));
    dtl_assert(dtt::is<dtt::Select>(static_cast<dtt::Node&>(select_node)));
}
