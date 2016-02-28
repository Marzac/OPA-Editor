/**
    OPA Editor: XML based program file
    XML based program file

    The MIT License (MIT)

    Source code copyright (c) 2013-2016 Frédéric Meslin
    Email: fredericmeslin@hotmail.com
    Website: www.fredslab.net
    Twitter: @marzacdev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#include "programfile.h"
#include "globals.h"

#include <QFile>
#include <QMessageBox>

/*****************************************************************************/
ProgramFile::ProgramFile(QString filename)
{
    this->filename = filename;
}

void ProgramFile::setFilename(QString filename)
{
    this->filename = filename;
}

/*****************************************************************************/
void ProgramFile::save(const OpaProgram * program)
{
// Open the file
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(NULL, "Error", "Impossible to write the file");
        return;
    }
// Write document header
    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeDTD("<!DOCTYPE OPA-Program>");
// Write program data
    writeProgram(stream, program);
    stream.writeEndElement();
// Close the file
    file.close();
}

void ProgramFile::load(OpaProgram * program)
{
// Open the file
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::critical(NULL, "Error", "Impossible to read the file");
        return;
    }
// Read the Program data
    QXmlStreamReader stream(&file);
    while (!stream.atEnd()) {
        stream.readNextStartElement();
        if (stream.name() == "OPA-Program")
            readProgram(stream, program);
    }
    if (stream.hasError()) {
    }
// Close the file
    file.close();
}

/*****************************************************************************/
void ProgramFile::readProgram(QXmlStreamReader &stream, OpaProgram * program)
{
    bool ok;
    while (stream.readNextStartElement()) {
        if (stream.name() == "name") {
            programNameFromQS(stream.readElementText(), program->params.name);
        }else if (stream.name() == "algorithm")
            program->params.algorithm = stream.readElementText().toInt(&ok);
        else if (stream.name() == "volume")
            program->params.volume = stream.readElementText().toInt(&ok);
        else if (stream.name() == "panning")
            program->params.panning = stream.readElementText().toInt(&ok);
        else if (stream.name() == "reserved")
            program->params.reserved = stream.readElementText().toInt(&ok);
        else if (stream.name() == "Operator") {
            int o = stream.attributes().value("id").toInt(&ok);
            if (o >= 0 && o < OPA_ALGOS_OP_NB)
                readOperator(stream, &program->opParams[o]);
        }
    }
}

void ProgramFile::readOperator(QXmlStreamReader &stream, OpaOperatorParams * params)
{
    bool ok;
    while (stream.readNextStartElement()) {
        if (stream.name() == "volume")
            params->volume = stream.readElementText().toInt(&ok);
        else if (stream.name() == "coarse")
            params->coarse = stream.readElementText().toInt(&ok);
        else if (stream.name() == "fine")
            params->fine = stream.readElementText().toInt(&ok);
        else if (stream.name() == "envAttack")
            params->envAttack = stream.readElementText().toInt(&ok);
        else if (stream.name() == "envDecay")
            params->envDecay = stream.readElementText().toInt(&ok);
        else if (stream.name() == "envSusLevel")
            params->envSusLevel = stream.readElementText().toInt(&ok);
        else if (stream.name() == "envIniLevel")
            params->envIniLevel = stream.readElementText().toInt(&ok);
        else if (stream.name() == "envRelease")
            params->envRelease = stream.readElementText().toInt(&ok);
        else if (stream.name() == "LFOSpeed")
            params->LFOSpeed = stream.readElementText().toInt(&ok);
        else if (stream.name() == "LFOAmount")
            params->LFOAmount = stream.readElementText().toInt(&ok);
        else if (stream.name() == "feedback")
            params->feedback = stream.readElementText().toInt(&ok);
        else if (stream.name() == "flags")
            params->flags = stream.readElementText().toInt(&ok);
    }
}

/*****************************************************************************/
void ProgramFile::writeProgram(QXmlStreamWriter &stream, const OpaProgram * program)
{
    QString value;
// Write the program header
    stream.writeStartElement("OPA-Program");
    stream.writeAttribute("version", "1.0");
// Write the program parameters
    programNameToQS(program->params.name, value);
    stream.writeTextElement("name",         value);
    stream.writeTextElement("algorithm",    value.setNum(program->params.algorithm));
    stream.writeTextElement("volume",       value.setNum(program->params.volume));
    stream.writeTextElement("panning",      value.setNum(program->params.panning));
    stream.writeTextElement("reserved",     value.setNum(program->params.reserved));
// Write the operators
    for (int i = 0; i < OPA_ALGOS_OP_NB; i++) {
        stream.writeStartElement("Operator");
        stream.writeAttribute("id",value.setNum(i));
        writeOperator(stream, &program->opParams[i]);
        stream.writeEndElement();
    }
    stream.writeEndElement();
}

void ProgramFile::writeOperator(QXmlStreamWriter &stream, const OpaOperatorParams * params)
{
    QString value;
    stream.writeTextElement("volume",      value.setNum(params->volume));
    stream.writeTextElement("coarse",      value.setNum(params->coarse));
    stream.writeTextElement("fine",        value.setNum(params->fine));
    stream.writeTextElement("envAttack",   value.setNum(params->envAttack));
    stream.writeTextElement("envDecay",    value.setNum(params->envDecay));
    stream.writeTextElement("envSusLevel", value.setNum(params->envSusLevel));
    stream.writeTextElement("envIniLevel", value.setNum(params->envIniLevel));
    stream.writeTextElement("envRelease",  value.setNum(params->envRelease));
    stream.writeTextElement("LFOSpeed",    value.setNum(params->LFOSpeed));
    stream.writeTextElement("LFOAmount",   value.setNum(params->LFOAmount));
    stream.writeTextElement("feedback",    value.setNum(params->feedback));
    stream.writeTextElement("flags",       value.setNum(params->flags));
}
