#ifndef ISUBJECT_HPP
#define ISUBJECT_HPP

#include "IObserver.hpp"
#include <memory>

// Interface Subject (Observado)
class ISubject {
public:
  virtual ~ISubject() = default;
  virtual void attach(std::shared_ptr<IObserver> observer) = 0;
  virtual void detach(std::shared_ptr<IObserver> observer) = 0;
  virtual void notify(const std::string &mensagem) = 0;
};

#endif // ISUBJECT_HPP
