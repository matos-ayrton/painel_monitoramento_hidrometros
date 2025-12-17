#ifndef IOBSERVER_HPP
#define IOBSERVER_HPP

#include <string>

// Interface Observer
class IObserver {
public:
  virtual ~IObserver() = default;
  virtual void update(const std::string &origem,
                      const std::string &mensagem) = 0;
};

#endif // IOBSERVER_HPP
