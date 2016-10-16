#include <catch.hpp>

#include "PcapFile.h"
#include "FsIOHelper.h"
#include "GlobalFsIOHelper.h"

class pcapIOFixture {
public:
    pcapIOFixture()
    : gfsioh(GlobalFsIOHelper::instance()) {
        gfsioh.registerPath("1.pcap", [] (std::stringstream& stream) {
            char data[] = "\xd4\xc3\xb2\xa1\x02\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x01\x00\x00\x00";
            stream.write(data, sizeof(data) - 1);
        }, [this] (const std::stringstream& stream) {
            (void)stream;
        });

        gfsioh.registerPath("2.pcap", [] (std::stringstream& stream) {
            char data[] = "\xa1\xb2\xc3\xd4\x00\x02\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x01";
            stream.write(data, sizeof(data) - 1);
        }, [this] (const std::stringstream& stream) {
            (void)stream;
        });

        gfsioh.registerPath("3.pcap", [] (std::stringstream& stream) {
            char data[] = "\xf1\xb2\xc3\xd4\x00\x02\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x01";
            stream.write(data, sizeof(data) - 1);
        }, [this] (const std::stringstream& stream) {
            (void)stream;
        });

        gfsioh.registerPath("4.pcap", [] (std::stringstream& stream) {
            char data[] = "\x02\x68";
            stream.write(data, sizeof(data) - 1);
        }, [this] (const std::stringstream& stream) {
            (void)stream;
        });

        gfsioh.registerPath("5.pcap", [] (std::stringstream& stream) {
            (void)stream;
        }, [this] (const std::stringstream& stream) {
            std::stringstream ss;
            char data[] = "\xd4\xc3\xb2\xa1\x02\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x01\x00\x00\x00";
            ss.write(data, sizeof(data) - 1);
            if (stream.str() != ss.str()) {
                throw std::runtime_error("Not intended datas.");
            }

        });
    };

    ~pcapIOFixture() {
        gfsioh.unregisterAll();
    };

protected:
    GlobalFsIOHelper& gfsioh;
};

TEST_CASE_METHOD(pcapIOFixture, "Pcap file tests", "[net][pcap][io]") {

    SECTION("Load a good magic number") {
        Net::PcapFile<FsIOHelper> pcap;
        REQUIRE_NOTHROW(pcap.loadFile("1.pcap"));
        CHECK(pcap.isByteSwap() == false);
        CHECK(pcap.versionNumber() == std::make_tuple(2, 4));
        CHECK(pcap.timezone() == 0);
        CHECK(pcap.accuracy() == 0);
        CHECK(pcap.maxLength() == 262144);
        CHECK(pcap.linkType() == 1);
    }

    SECTION("Load a reverse magic number") {
        Net::PcapFile<FsIOHelper> pcap;
        REQUIRE_NOTHROW(pcap.loadFile("2.pcap"));
        CHECK(pcap.isByteSwap() == true);
        CHECK(pcap.versionNumber() == std::make_tuple(2, 4));
        CHECK(pcap.timezone() == 0);
        CHECK(pcap.accuracy() == 0);
        CHECK(pcap.maxLength() == 262144);
        CHECK(pcap.linkType() == 1);
    }

    SECTION("Load wrong magic") {
        Net::PcapFile<FsIOHelper> pcap;
        CHECK_THROWS_AS(pcap.loadFile("3.pcap"), std::runtime_error);
    }

    SECTION("Load not even large enough for header") {
        Net::PcapFile<FsIOHelper> pcap;
        CHECK_THROWS_AS(pcap.loadFile("4.pcap"), std::runtime_error);
    }

    SECTION("Save file") {
        Net::PcapFile<FsIOHelper> pcap;
        pcap.versionNumber(2, 4);
        pcap.timezone(0);
        pcap.accuracy(0);
        pcap.maxLength(262144);
        pcap.linkType(1);
        CHECK_NOTHROW(pcap.saveFile("5.pcap"));
    }
}
