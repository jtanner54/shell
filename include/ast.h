#ifndef AST_H
#define AST_H

#include <memory>
#include <string>
#include <vector>

// base class w/ polymorphic behavior for all AST nodes
struct ASTNode {
  // virtual destructor for proper cleanup of derived classes
  // virtual for base class and redefined in derived classes
  virtual ~ASTNode() = default;
  virtual int execute() = 0;
};

// simple command w/ optional redirection
struct SimpleCommandNode : public ASTNode {
  std::vector<std::string> args;
  std::string redirect_out;
  //std::string redirect_in;
  bool append_out = false; // >> vs >
  int redirect_out_fd = 1;

  int execute() override;
};

// | operator
struct PipelineNode : public ASTNode {
  std::unique_ptr<ASTNode> left;
  std::unique_ptr<ASTNode> right;

  int execute() override;
};

// && or ||
struct LogicalNode : public ASTNode {
  enum class Type { AND, OR };
  Type type;
  std::unique_ptr<ASTNode> left;
  std::unique_ptr<ASTNode> right;

  int execute() override;
};

#endif  // AST_H