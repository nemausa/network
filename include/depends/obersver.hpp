/**
* @file obersver.hpp
*
* @brief This message displayed in Doxygen Files index
*
* @ingroup PackageName
* (note: this needs exactly one @defgroup somewhere)
*
* @date	2021-06-20-17-46
* @author Nemausa
* @contact: tappanmorris@outlook.com
*
*/
#ifndef OBERSVER
#define OBERSVER


#include <iostream>
#include <string>
#include <list>
#include <atomic>

namespace nemausa {
namespace io {

class i_observer {
public:
    virtual ~i_observer(){};
    virtual void update(const std::string &message_from_subject) = 0;
};

class i_subject{
public:
    virtual ~i_subject(){};
    virtual void attach(i_observer *observer) = 0;
    virtual void detach(i_observer *observer) = 0;
    virtual void notify() = 0;
};


class subject : public i_subject{
public:
    virtual ~subject(){
        std::cout << "goodby, I was the subject.\n";
    }

    void attach(i_observer *observer) override {
        list_observer_.push_back(observer);
    }

    void detach(i_observer *observer) override {
        list_observer_.remove(observer);
    }

    void notify() override {
        std::list<i_observer *>::iterator iterator = list_observer_.begin();
        // how_many_observer();
        while (iterator != list_observer_.end())
        {
            (*iterator)->update(message_);
            ++iterator;
        }
        
    }

    void create_message(std::string message = "empty"){
        this->message_ = message;
        notify();
    }
    
    void how_many_observer() {
        std::cout << "There are " << list_observer_.size() << " observers in the list\n";
    }

    void some_business_logic() {
        this->message_ = "change message ";
        notify();
        std::cout << "I'm about to do some thing important\n";
    }

private:
    std::list<i_observer *> list_observer_;
    std::string message_;
};


class observer : public i_observer{
public:
    observer(subject &subject) : subject_(subject) {
        msg_count_ = 0;
        clients_count_ = 0;
        recv_count_ = 0;
        this->subject_.attach(this);
        std::cout << "Hi, I'm the observer \"" << ++observer::static_number_ << "\".\n";
        this->number_ = observer::static_number_;
    }

    virtual ~observer() {
        std::cout << "Goodbye, I was the observer \"" << this->number_ << "\".\n";
    }

    void update(const std::string &message_from_subject) override {
        message_from_subject_ = message_from_subject;
        // print_info();
        if (message_from_subject == "msg") {
            msg_count_++;
        } else if (message_from_subject == "recv") {
            recv_count_++;
        } else if (message_from_subject == "join") {
            clients_count_++;
        } else if (message_from_subject == "leave") {
            clients_count_--;
        }
    }

    void remove_me_from_subject() {
        subject_.detach(this);
        std::cout << "Observer \"" << number_ << "\" removed from the list.\n";
    }

    void print_info() {
        std::cout << "Observer \"" << this->number_ << "\": a new message is available -->"
            << this->message_from_subject_ << "\n";
    }

    int client_count() {
        return clients_count_;
    }

    void client_count(int _client_count) {
        clients_count_ = _client_count;
    }

    int msg_count() {
        return msg_count_;
    }

    void msg_count(int _msg_count) {
        msg_count_ = _msg_count;
    }

    int recv_count() {
        return recv_count_;
    }

    void recv_count(int _recv_count) {
        recv_count_ = _recv_count;
    }
    
private: 
    std::string message_from_subject_;
    subject &subject_;
    static int static_number_;
    std::atomic<int> msg_count_;
    std::atomic<int> clients_count_;
    std::atomic<int> recv_count_;
    int number_;
};

} // namespace io 
} // namespace nemausa

#endif // OBERSVER