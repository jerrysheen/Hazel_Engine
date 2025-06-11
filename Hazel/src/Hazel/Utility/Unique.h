#pragma once
#include "boost/uuid/uuid.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
namespace Hazel
{
	class Unique 
	{
    private:
        static Ref<Unique> instance;


    public :

        // ��̬������ֱ��ͨ����������
        static boost::uuids::uuid GetUUID() {
            static boost::uuids::random_generator generator;
            return generator();
        }
	};

}