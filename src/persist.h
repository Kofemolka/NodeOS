#ifndef PERSIST_H
#define PERSIST_H

#include <EEPROM.h>
#include <map>

class Persist
{
    Persist() : _size(0)
    {
      EEPROM.begin(512);
    }

    static Persist* _inst;
public:
    static Persist& Inst()
    {
      if(_inst == 0)
      {
        _inst = new Persist();
      }

      return *_inst;
    }

    template<typename T>
    int Register(const String& k, T& t)
    {
        std::map<String,int>::iterator iter = _addr.find(k);
        if(iter == _addr.end())
        {
            size_t s = sizeof(T);
            int addr = _size;
            _addr.insert(std::pair<String,int>(k,_size));
            _size += s;

            return addr;
        }
        
        return iter->second;
    }

    template<typename T>
    T& Get(const String& k, T& t) {
      return EEPROM.get(Register(k, t), t);
    }

    template<typename T>
    const T& Put(const String& k, const T& t) {
      const T& ret = EEPROM.put(Register(k, t), t);
      EEPROM.commit();
      return ret;
    }

private:
    std::map<String, int> _addr;
    int _size;
};

#endif
