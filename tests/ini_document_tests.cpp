#include "../modern/IniDocument.h"
#include <QCoreApplication>
#include <QFile>
#include <QTemporaryDir>
#include <QDebug>
static void verify(bool v, const char* why) { if (!v) qFatal("Test failed: %s", why); }
int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QTemporaryDir dir;
    verify(dir.isValid(), "temp dir");
    const QString path = dir.filePath("rules.ini");
    const QByteArray sample = "; comment\r\n[InfantryTypes]\r\n1=E1\r\n[E1]\r\nPrimary=M1 ; keep me\r\n[M1]\r\nDamage=10\r\nROF=2\r\n";
    { QFile f(path); verify(f.open(QIODevice::WriteOnly), "open fixture"); verify(f.write(sample) == sample.size(), "write"); }
    IniDocument d; QString error;
    verify(d.load(path, IniDocument::Encoding::Utf8, &error), "load");
    verify(d.sections().size() == 3, "section count");
    verify(d.category(d.indexOf("e1")) == "Infantry", "categorization");
    verify(d.category(d.indexOf("M1")) == "Weapon", "weapon heuristic");
    verify(d.outgoing(d.indexOf("E1")).contains(d.indexOf("M1")), "outbound");
    verify(d.incoming(d.indexOf("M1")).contains(d.indexOf("E1")), "inbound");
    verify(d.save(path, &error), "save");
    { QFile f(path); verify(f.open(QIODevice::ReadOnly), "reopen"); verify(f.readAll() == sample, "unchanged roundtrip bytes"); }
    verify(d.setKey(d.indexOf("E1"), "Primary", "M2"), "edit value");
    verify(d.addSection("M2"), "add section");
    verify(d.renameSection(d.indexOf("M2"), "Weapon2"), "rename section");
    verify(d.renameKey(d.indexOf("E1"), "Primary", "Secondary"), "rename key");
    verify(d.save(path, &error), "save edit");
    IniDocument second;
    verify(second.load(path, IniDocument::Encoding::Utf8, &error), "reload");
    verify(second.sections().size() == 4, "new section persisted");
    verify(second.lines().join("\n").contains("; keep me"), "inline comments preserved");
    verify(second.deleteSection(second.indexOf("Weapon2")), "delete section");
    verify(second.removeKey(second.indexOf("E1"), "Secondary"), "delete key");
    qInfo() << "INI document tests passed";
}
