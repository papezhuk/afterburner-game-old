#include "projectInterface_gamelib.h"

IProjectInterface& ProjectInterface_Server::StaticInstance()
{
	static ProjectInterface_Server instance;
	return instance;
}

IFileLoader& ProjectInterface_Server::FileLoader()
{
	return m_FileLoader;
}

ILogInterface& ProjectInterface_Server::LogInterface()
{
	return m_LogInterface;
}

#ifdef CLIENT_DLL
IProjectInterface& ProjectInterface_Client::StaticInstance()
{
	static ProjectInterface_Client instance;
	return instance;
}

IFileLoader& ProjectInterface_Client::FileLoader()
{
	return m_FileLoader;
}

ILogInterface& ProjectInterface_Client::LogInterface()
{
	return m_LogInterface;
}

#endif
