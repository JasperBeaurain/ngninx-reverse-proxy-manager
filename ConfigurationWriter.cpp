#include "ConfigurationWriter.h"

#include <iostream>
#include <fstream>
#include <string>
#include <Exceptions/FileOpenException.h>

using std::string;

void ConfigurationWriter::write()
{
    std::ifstream inputStream;
    inputStream.open(this->templateFileName);

    if(! inputStream.is_open())
    {
        throw FileOpenException(this->templateFileName);
    }

    string outputFileName = this->service.getName().append(".conf");

    std::ofstream outputStream;
    outputStream.open(outputFileName);

    if(! outputStream.is_open())
    {
        throw FileOpenException(outputFileName);
    }

    string line;

    while(getline(inputStream, line))
    {
        this->writeName(line);
        this->writeServerNames(line);
        this->writeUpstreams(line);

        outputStream << line << std::endl;
    }

    inputStream.close();
    outputStream.close();
}

ConfigurationWriter::ConfigurationWriter(Service &service) : ConfigurationWriter(service, "template.conf")
{
}

ConfigurationWriter::ConfigurationWriter(Service &service, std::string templateFileName) : service(service), templateFileName(templateFileName)
{
}

void ConfigurationWriter::writeUpstreams(string &line)
{
    string upstreams;

    for(Destination &destination : this->service.getDestinations())
    {
        upstreams = upstreams.append("server ").append(destination.toString()).append("\n");
    }

    this->replaceInLine(line, "{{UPSTREAMS}}", upstreams);
}

void ConfigurationWriter::writeName(string &line)
{
    this->replaceInLine(line, "{{NAME}}", this->service.getName());
}

void ConfigurationWriter::writeServerNames(string &line)
{
    string hostnames;

    for(const string &hostname : this->service.getHostnames())
    {
        hostnames = hostnames.append(hostname).append(" ");
    }

    //remove last space
    if(hostnames.length() > 0)
    {
        hostnames = hostnames.erase(hostnames.length() - 1, 1);
    }

    this->replaceInLine(line, "{{SERVERNAMES}}", hostnames);
}

void ConfigurationWriter::replaceInLine(string &line, string original, string replacement)
{
    std::size_t position = line.find(original);
    if(position != string::npos)
    {
        line.replace(position, string(original).length(), replacement);
    }
}
