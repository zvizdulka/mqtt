#include <mosquitto.h>
#include <errno.h>
#include <system_error>
#include <iostream>
#include <new>
using namespace std;

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg){
    cout << "Topic: " << msg ->topic << ". Message: " << (char*) msg ->payload << endl;
}

int main() {
    try{
        mosquitto_lib_init();
        struct mosquitto *m = mosquitto_new(NULL, true, NULL);
        if(m == 0){
            if(errno == ENOMEM){
                throw bad_alloc();
            }
            if(errno == EINVAL){
                throw invalid_argument("mosquitto_new.");
            }
        }
        mosquitto_message_callback_set(m, on_message);
        int rc = mosquitto_connect(m, "localhost", 1883, 10);
        if(rc != MOSQ_ERR_SUCCESS){
            if(rc == MOSQ_ERR_INVAL){
                throw invalid_argument("mosquitto_connect.");
            }
            if(rc == MOSQ_ERR_ERRNO){
                char buffer[256];
                strerror_r(errno, buffer, 256);
                throw runtime_error(buffer);
            }
        }
        rc = mosquitto_loop_start(m);
        if(rc != MOSQ_ERR_SUCCESS){
            if(rc == MOSQ_ERR_INVAL){
                throw invalid_argument("mosquitto_loop_start.");
            }
            if(rc == MOSQ_ERR_NOT_SUPPORTED){
                throw runtime_error("mosquitto_loop_start.");
            }
        }
        string s;
        cout << "Select a command: sub or pub. To exit, press end." << endl;
        while(true){
            cin >> s;
            if(s == "end"){
                break;
            }
            if(s == "pub"){
                string topic;
                string message;
                int qos;
                cout << "Enter topic, qos: " ;
                cin >> topic >> qos;
                cout << "Enter message: ";
                cin.ignore();
                getline(cin, message);
                int status_pub = mosquitto_publish(m, NULL, topic.c_str(), message.size(), message.c_str(), qos, false);
                if(status_pub != MOSQ_ERR_SUCCESS){
                    if(status_pub == MOSQ_ERR_INVAL){
                        throw invalid_argument("mosquitto_publish.");
                    }
                    if(status_pub == MOSQ_ERR_NOMEM){
                        throw bad_alloc();
                    }
                    if(status_pub == MOSQ_ERR_NO_CONN){
                        throw runtime_error("mosquitto_publish. Client isn’t connected to a broker.");
                    }
                    if(status_pub == MOSQ_ERR_PROTOCOL){
                        throw runtime_error("mosquitto_publish. Protocol error communicating with the broker.");
                    }
                    if(status_pub == MOSQ_ERR_PAYLOAD_SIZE){
                        throw length_error("mosquitto_publish.");
                    }
                    if(status_pub == MOSQ_ERR_MALFORMED_UTF8){
                        throw runtime_error("mosquitto_publish. Topic is not valid UTF-8.");
                    }
                    if(status_pub == MOSQ_ERR_QOS_NOT_SUPPORTED){
                        throw runtime_error("mosquitto_publish. QoS is greater than that supported by the broker.");
                    }
                    if(status_pub == MOSQ_ERR_PAYLOAD_SIZE){
                        throw overflow_error("mosquitto_publish.");
                    }
                }
            }
            if(s == "sub"){
                string topic;
                int qos;
                cout << "Enter topic, qos: ";
                cin >> topic >> qos;
                int status_sub = mosquitto_subscribe(m, NULL, topic.c_str(), qos);
                if(status_sub != MOSQ_ERR_SUCCESS){
                    if(status_sub == MOSQ_ERR_INVAL){
                        throw invalid_argument("mosquitto_subscribe.");
                    }
                    if(status_sub == MOSQ_ERR_NOMEM){
                        throw bad_alloc();
                    }
                    if(status_sub == MOSQ_ERR_NO_CONN){
                        throw runtime_error("mosquitto_subscribe. Client isn’t connected to a broker.");
                    }
                    if(status_sub == MOSQ_ERR_MALFORMED_UTF8){
                        throw runtime_error("mosquitto_subscribe. Topic is not valid UTF-8.");
                    }
                    if(status_sub == MOSQ_ERR_PAYLOAD_SIZE){
                        throw overflow_error("mosquitto_subscribe.");
                    }
                }
            }
        }
        rc = mosquitto_loop_stop(m, true);
        if(MOSQ_ERR_SUCCESS != rc){
            if(rc == MOSQ_ERR_INVAL){
                throw invalid_argument("mosquitto_loop_stop.");
            }
            if(rc == MOSQ_ERR_NOT_SUPPORTED){
                throw runtime_error("mosquitto_loop_stop.");
            }
        }
        rc = mosquitto_disconnect(m);
        if(MOSQ_ERR_SUCCESS != rc){
            if(rc == MOSQ_ERR_INVAL){
                throw invalid_argument("mosquitto_disconnect.");
            }
            if(rc == MOSQ_ERR_NO_CONN){
                throw runtime_error("mosquitto_disconnect. Client isn’t connected to a broker.");
            }
        }
        mosquitto_destroy(m);
        mosquitto_lib_cleanup();
    }
    catch(invalid_argument err){
        cout << "Invalid argument: " << err.what() << endl;
    }
    catch(overflow_error err){
        cout << "Overflow error: " << err.what() << endl;
    }
    catch(length_error err){
        cout << "Lenght error: " << err.what() << endl;
    }
    catch(runtime_error err){
        cout << err.what() << endl;
    }
    catch (bad_alloc err){
        char buffer[256];
        strerror_r(errno, buffer, 256);
        cout << buffer << err.what() << endl;
    }
}
