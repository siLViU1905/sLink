#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <message/Message.h>
#include <protocol/Command.h>

TEST_CASE("Message default constructor", "[message]")
{
    sLink::message::Message msg;

    REQUIRE(msg.getContent().empty());
    REQUIRE(msg.getSenderName().empty());
}

TEST_CASE("Message parameterised constructor stores values", "[message]")
{
    sLink::message::Message msg(sLink::protocol::Command::CHAT_MESSAGE, "alice", "Hello, world!");

    CHECK(msg.getCommand() == sLink::protocol::Command::CHAT_MESSAGE);
    CHECK(msg.getSenderName() == "alice");
    CHECK(msg.getContent()    == "Hello, world!");
}

TEST_CASE("Message setters mutate the stored data", "[message]")
{
    sLink::message::Message msg;

    msg.setCommand(sLink::protocol::Command::LOGIN_REQUEST);
    msg.setSenderName("bob");
    msg.setContent("ping");

    CHECK(msg.getCommand()    == sLink::protocol::Command::LOGIN_REQUEST);
    CHECK(msg.getSenderName() == "bob");
    CHECK(msg.getContent()    == "ping");
}

TEST_CASE("Message round-trips through serialize/deserialize", "[message][serialization]")
{
    sLink::message::Message original(sLink::protocol::Command::CHAT_MESSAGE, "carol", "Round-trip test");

    const std::string raw = original.serialize();

    REQUIRE_FALSE(raw.empty());

    sLink::message::Message restored = sLink::message::Message::deserialize(raw);

    CHECK(restored.getCommand()    == original.getCommand());
    CHECK(restored.getSenderName() == original.getSenderName());
    CHECK(restored.getContent()    == original.getContent());
    CHECK(restored.getTimestamp().getMs() == original.getTimestamp().getMs());
}

TEST_CASE("Serialized message is valid JSON string", "[message][serialization]")
{
    sLink::message::Message msg(sLink::protocol::Command::LOGIN_REQUEST, "dave", "");

    const std::string raw = msg.serialize();

    REQUIRE_THAT(raw, Catch::Matchers::StartsWith("{"));
    REQUIRE_THAT(raw, Catch::Matchers::EndsWith("}"));
}

TEST_CASE("All Command values survive round-trip", "[message][serialization]")
{
    using C = sLink::protocol::Command;

    const std::vector<C> commands = {
        C::LOGIN_REQUEST,
        C::LOGIN_RESPONSE_REJECT,
        C::LOGIN_RESPONSE_ACCEPT,
        C::CHAT_MESSAGE,
        C::USER_JOINED,
        C::USER_LEFT
    };

    for (auto cmd : commands)
    {
        sLink::message::Message msg(cmd, "user", "body");
        auto restored = sLink::message::Message::deserialize(msg.serialize());
        CHECK(restored.getCommand() == cmd);
    }
}

TEST_CASE("Message with empty content serializes cleanly", "[message][serialization]")
{
    sLink::message::Message msg(sLink::protocol::Command::LOGIN_RESPONSE_ACCEPT, "server", "");

    const auto raw= msg.serialize();
    const auto restored = sLink::message::Message::deserialize(raw);

    CHECK(restored.getContent().empty());
}

TEST_CASE("Message with special characters in content", "[message][serialization]")
{
    const std::string special = R"(Hello "world" \ / < > & ')";

    sLink::message::Message msg(sLink::protocol::Command::CHAT_MESSAGE, "eve", special);
    auto restored = sLink::message::Message::deserialize(msg.serialize());

    CHECK(restored.getContent() == special);
}