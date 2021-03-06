//
//  main.cpp
//  Mustache
//
//  Created by Kevin Wojniak on 4/11/15.
//  Copyright (c) 2015 Kevin Wojniak. All rights reserved.
//

#include "mustache.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("variables") {

    SECTION("empty") {
        Mustache::Mustache<std::string> tmpl("");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data).empty());
    }

    SECTION("none") {
        Mustache::Mustache<std::string> tmpl("Hello");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data) == "Hello");
    }

    SECTION("single_miss") {
        Mustache::Mustache<std::string> tmpl("Hello {{name}}");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data) == "Hello ");
    }

    SECTION("single_exist") {
        Mustache::Mustache<std::string> tmpl("Hello {{name}}");
        Mustache::Data<std::string> data;
        data.set("name", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve");
    }

    SECTION("escape") {
        Mustache::Mustache<std::string> tmpl("Hello {{name}}");
        Mustache::Data<std::string> data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello &quot;S&quot;&lt;br&gt;te&amp;v&apos;e");
    }
    
    SECTION("unescaped1") {
        Mustache::Mustache<std::string> tmpl("Hello {{{name}}}");
        Mustache::Data<std::string> data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }

    SECTION("unescaped2") {
        Mustache::Mustache<std::string> tmpl("Hello {{&name}}");
        Mustache::Data<std::string> data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }

    SECTION("unescaped2_spaces") {
        Mustache::Mustache<std::string> tmpl("Hello {{   &      name  }}");
        Mustache::Data<std::string> data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }
    
    SECTION("empty_name") {
        Mustache::Mustache<std::string> tmpl("Hello {{}}");
        Mustache::Data<std::string> data;
        data.set("", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve");
    }

    SECTION("braces") {
        Mustache::Mustache<std::string> tmpl("my {{var}}");
        Mustache::Data<std::string> data;
        data.set("var", "{{te}}st");
        CHECK(tmpl.render(data) == "my {{te}}st");
    }

}

TEST_CASE("comments") {
    
    SECTION("simple") {
        Mustache::Mustache<std::string> tmpl("<h1>Today{{! ignore me }}.</h1>");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data) == "<h1>Today.</h1>");
    }

    SECTION("newlines") {
        Mustache::Mustache<std::string> tmpl("Hello\n{{! ignore me }}\nWorld\n");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data) == "Hello\n\nWorld\n");
    }

}

TEST_CASE("set_delimiter") {

    SECTION("basic") {
        Mustache::Mustache<std::string> tmpl("{{name}}{{=<% %>=}}<% name %><%={{ }}=%>{{ name }}");
        Mustache::Data<std::string> data;
        data.set("name", "Steve");
        CHECK(tmpl.render(data) == "SteveSteveSteve");
    }

    SECTION("small") {
        Mustache::Mustache<std::string> tmpl("{{n}}{{=a b=}}anba={{ }}=b{{n}}");
        Mustache::Data<std::string> data;
        data.set("n", "s");
        CHECK(tmpl.render(data) == "sss");
    }
    
    SECTION("noreset") {
        Mustache::Mustache<std::string> tmpl("{{=[ ]=}}[name] [x] + [y] = [sum]");
        Mustache::Data<std::string> data;
        data.set("name", "Steve");
        data.set("x", "1");
        data.set("y", "2");
        data.set("sum", "3");
        CHECK(tmpl.render(data) == "Steve 1 + 2 = 3");
    }
    
    SECTION("whitespace") {
        Mustache::Mustache<std::string> tmpl("|{{= @   @ =}}|");
        Mustache::Data<std::string> data;
        REQUIRE(tmpl.isValid());
        CHECK(tmpl.render(data) == "||");
    }

}

TEST_CASE("sections") {
    
    using Data = Mustache::Data<std::string>;

    SECTION("nonexistant") {
        Mustache::Mustache<std::string> tmpl("{{#var}}not shown{{/var}}");
        Data data;
        CHECK(tmpl.render(data) == "");
    }

    SECTION("false") {
        Mustache::Mustache<std::string> tmpl("{{#var}}not shown{{/var}}");
        Data data;
        data.set("var", Data(Data::Type::False));
        CHECK(tmpl.render(data) == "");
    }

    SECTION("emptylist") {
        Mustache::Mustache<std::string> tmpl("{{#var}}not shown{{/var}}");
        Data data;
        data.set("var", Data(Data::Type::List));
        CHECK(tmpl.render(data) == "");
    }
    
    SECTION("nested") {
        Mustache::Mustache<std::string> tmpl("{{#var1}}hello{{#var2}}world{{/var2}}{{/var1}}");
        Data data;
        data.set("var1", Data::Type::True);
        data.set("var2", Data::Type::True);
        CHECK(tmpl.render(data) == "helloworld");
    }

}

TEST_CASE("sections_inverted") {
    
    using Data = Mustache::Data<std::string>;
    
    SECTION("nonexistant") {
        Mustache::Mustache<std::string> tmpl("{{^var}}shown{{/var}}");
        CHECK(tmpl.render(Data()) == "shown");
    }
    
    SECTION("false") {
        Mustache::Mustache<std::string> tmpl("{{^var}}shown{{/var}}");
        Data data("var", Data(Data::Type::False));
        CHECK(tmpl.render(data) == "shown");
    }
    
    SECTION("emptylist") {
        Mustache::Mustache<std::string> tmpl("{{^var}}shown{{/var}}");
        Data data("var", Data(Data::Type::List));
        CHECK(tmpl.render(data) == "shown");
    }
    
}

TEST_CASE("section_lists") {
    
    using Data = Mustache::Data<std::string>;
    
    SECTION("list") {
        Mustache::Mustache<std::string> tmpl("{{#people}}Hello {{name}}, {{/people}}");
        Data people = Data::List();
        for (auto& name : {"Steve", "Bill", "Tim"}) {
            people.push_back(Data("name", name));
        }
        Data data("people", people);
        CHECK(tmpl.render(data) == "Hello Steve, Hello Bill, Hello Tim, ");
    }
    
    SECTION("nested") {
        Mustache::Mustache<std::string> tmpl("{{#families}}surname={{surname}}, members={{#members}}{{given}},{{/members}}|{{/families}}");
        Data families = Data::List();
        Data family1;
        family1.set("surname", "Smith");
        Data members1 = Data::List();
        Data member1a; member1a.set("given", "Steve"); members1.push_back(member1a);
        Data member1b; member1b.set("given", "Joe"); members1.push_back(member1b);
        family1.set("members", members1);
        Data family2;
        family2.set("surname", "Lee");
        Data members2 = Data::List();
        Data member2a; member2a.set("given", "Bill"); members2.push_back(member2a);
        Data member2b; member2b.set("given", "Peter"); members2.push_back(member2b);
        family2.set("members", members2);
        families.push_back(family1);
        families.push_back(family2);
        Data data;
        data.set("families", families);
        CHECK(tmpl.render(data) == "surname=Smith, members=Steve,Joe,|surname=Lee, members=Bill,Peter,|");
    }
    
    SECTION("dot") {
        Mustache::Mustache<std::string> tmpl("{{#names}}Hello {{.}}, {{/names}}");
        Data names = Data::List();
        names.push_back(Data("Steve"));
        names.push_back(Data("Bill"));
        names.push_back(Data("Tim"));
        Data data("names", names);
        CHECK(tmpl.render(data) == "Hello Steve, Hello Bill, Hello Tim, ");
    }

    SECTION("dot2") {
        Mustache::Mustache<std::string> tmpl("{{#names}}Hello {{.}}{{/names}}{{#friends}} and {{.}}{{/friends}}");
        Data friends = Data::List();
        friends.push_back("Bill");
        friends.push_back("Tim");
        Data data;
        data.set("friends", friends);
        CHECK(tmpl.render(data) == " and Bill and Tim");
        data.set("names", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve and Bill and Tim");
    }

}

TEST_CASE("section_object") {
    
    using Data = Mustache::Data<std::string>;
    
    SECTION("basic") {
        Mustache::Mustache<std::string> tmpl("{{#employee}}name={{name}}, age={{age}}{{/employee}}");
        Data person;
        person.set("name", "Steve");
        person.set("age", "42");
        Data data;
        data.set("employee", person);
        CHECK(tmpl.render(data) == "name=Steve, age=42");
    }

    SECTION("basic_parent") {
        Mustache::Mustache<std::string> tmpl("({{subject}}) {{#employee}}name={{name}}, age={{age}} - {{subject}}{{/employee}}");
        Data person;
        person.set("name", "Steve");
        person.set("age", "42");
        person.set("subject", "email");
        Data data;
        data.set("subject", "test");
        data.set("employee", person);
        CHECK(tmpl.render(data) == "(test) name=Steve, age=42 - email");
    }

}

TEST_CASE("examples") {
    
    SECTION("one") {
        Mustache::Mustache<std::string> tmpl{"Hello {{what}}!"};
        std::cout << tmpl.render({"what", "World"}) << std::endl;
		CHECK(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render({"what", "World"}) == "Hello World!");
    }

    SECTION("two") {
        using Data = Mustache::Data<std::string>;
        Mustache::Mustache<std::string> tmpl{"{{#employees}}{{name}}, {{/employees}}"};
        Data employees{Data::List()};
        employees << Data{"name", "Steve"} << Data{"name", "Bill"};
        tmpl.render({"employees", employees}, std::cout) << std::endl;
		CHECK(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render({"employees", employees}) == "Steve, Bill, ");
    }

	SECTION("three") {
		Mustache::Mustache<std::string> tmpl("Hello {{what}}!");
		std::stringstream ss;
		tmpl.render({"what", "World"}, [&ss](const std::string& str) {
			ss << str;
		});
		CHECK(tmpl.isValid());
		CHECK(tmpl.errorMessage() == "");
		CHECK(ss.str() == "Hello World!");
	}

}

TEST_CASE("data") {
    
    using Data = Mustache::Data<std::string>;

    SECTION("types") {
        Data data("age", "42");
        data["name"] = "Steve";
        data["is_human"] = Data::Type::True;
        const Data* name;
        const Data* age;
        const Data* is_human;
        name = data.get("name");
        age = data.get("age");
        is_human = data.get("is_human");
        CHECK(name != 0);
        CHECK(age != 0);
        CHECK(is_human != 0);
        CHECK(data.get("miss") == 0);
        REQUIRE(name->isString());
        CHECK(name->stringValue() == "Steve");
        REQUIRE(age->isString());
        CHECK(age->stringValue() == "42");
        CHECK(is_human->isTrue());
    }

    SECTION("move_ctor") {
        Data obj1{Data::List()};
        CHECK(obj1.isList());
        Data obj2{std::move(obj1)};
        CHECK(obj2.isList());
        CHECK(obj1.type() == Data::Type::Invalid);
        obj2.push_back({"name", "Steve"}); // this should puke if the internal data isn't setup correctly
    }

    SECTION("move_assign") {
        Data obj1{Data::List()};
        CHECK(obj1.isList());
        Data obj2 = std::move(obj1);
        CHECK(obj2.isList());
        CHECK(obj1.type() == Data::Type::Invalid);
        obj2.push_back({"name", "Steve"}); // this should puke if the internal data isn't setup correctly
    }

}

TEST_CASE("errors") {

    SECTION("unclosed_section") {
        Mustache::Mustache<std::string> tmpl("test {{#employees}}");
        CHECK_FALSE(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "Unclosed section \"employees\" at 5");
    }

    SECTION("unclosed_tag") {
        Mustache::Mustache<std::string> tmpl("test {{employees");
        CHECK_FALSE(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "Unclosed tag at 5");
    }
    
    SECTION("unopened_section") {
        Mustache::Mustache<std::string> tmpl("test {{/employees}}");
        CHECK_FALSE(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "Unopened section \"employees\" at 5");
    }
    
    SECTION("invalid_set_delimiter") {
        Mustache::Mustache<std::string> tmpl("test {{=< =}}");
        CHECK_FALSE(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "Invalid set delimiter tag at 5");
    }
    
    SECTION("lambda") {
        using Data = Mustache::Data<std::string>;
        Mustache::Mustache<std::string> tmpl{"Hello {{lambda}}!"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string&){
            return "{{#what}}";
        }}});
        CHECK(tmpl.isValid() == true);
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render(data) == "Hello ");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed section \"what\" at 0");
    }

    SECTION("partial") {
        using Data = Mustache::Data<std::string>;
        Mustache::Mustache<std::string> tmpl{"Hello {{>partial}}!"};
        Data data("partial", Data{Data::PartialType{[](){
            return "{{#what}}";
        }}});
        CHECK(tmpl.isValid() == true);
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render(data) == "Hello ");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed section \"what\" at 0");
    }

}

TEST_CASE("partials") {

    using Data = Mustache::Data<std::string>;

    SECTION("empty") {
        Mustache::Mustache<std::string> tmpl{"{{>header}}"};
        Data data;
        CHECK(tmpl.render(data) == "");
    }

    SECTION("basic") {
        Mustache::Mustache<std::string> tmpl{"{{>header}}"};
        Data::PartialType partial = []() {
            return "Hello World";
        };
        Data data("header", Data{partial});
        CHECK(tmpl.render(data) == "Hello World");
    }

    SECTION("context") {
        Mustache::Mustache<std::string> tmpl{"{{>header}}"};
        Data::PartialType partial{[]() {
            return "Hello {{name}}";
        }};
        Data data("header", Data{partial});
        data["name"] = "Steve";
        CHECK(tmpl.render(data) == "Hello Steve");
    }
    
    SECTION("nested") {
        Mustache::Mustache<std::string> tmpl{"{{>header}}"};
        Data::PartialType header{[]() {
            return "Hello {{name}} {{>footer}}";
        }};
        Data::PartialType footer{[]() {
            return "Goodbye {{#names}}{{.}}|{{/names}}";
        }};
        Data names{Data::List()};
        names.push_back("Jack");
        names.push_back("Jill");
        Data data("header", header);
        data["name"] = "Steve";
        data["footer"] = Data{footer};
        data["names"] = Data{names};
        CHECK(tmpl.render(data) == "Hello Steve Goodbye Jack|Jill|");
    }

    SECTION("dotted") {
        Mustache::Mustache<std::string> tmpl{"{{>a.b}}"};
        Data::PartialType a_b{[]() {
            return "test";
        }};
        Data data("a.b", a_b);
        CHECK(tmpl.render(data) == "test");
    }
}

TEST_CASE("lambdas") {
    
    using Data = Mustache::Data<std::string>;
    
    SECTION("basic") {
        Mustache::Mustache<std::string> tmpl{"{{lambda}}"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string&){
            return "Hello {{planet}}";
        }}});
        data["planet"] = "world";
        CHECK(tmpl.render(data) == "Hello world");
    }

    SECTION("delimiters") {
        Mustache::Mustache<std::string> tmpl{"{{= | | =}}Hello, (|&lambda|)!"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string&){
            return "|planet| => {{planet}}";
        }}});
        data["planet"] = "world";
        CHECK(tmpl.render(data) == "Hello, (|planet| => world)!");
    }

    SECTION("nocaching") {
        Mustache::Mustache<std::string> tmpl{"{{lambda}} == {{{lambda}}} == {{lambda}}"};
        int calls = 0;
        Data data("lambda", Data{Data::LambdaType{[&calls](const std::string&){
            ++calls;
            return std::to_string(calls);
        }}});
        CHECK(tmpl.render(data) == "1 == 2 == 3");
    }

    SECTION("escape") {
        Mustache::Mustache<std::string> tmpl{"<{{lambda}}{{{lambda}}}"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string&){
            return ">";
        }}});
        CHECK(tmpl.render(data) == "<&gt;>");
    }
    
    SECTION("section") {
        Mustache::Mustache<std::string> tmpl{"<{{#lambda}}{{x}}{{/lambda}}>"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string& text){
            return text == "{{x}}" ? "yes" : "no";
        }}});
        CHECK(tmpl.render(data) == "<yes>");
    }

    SECTION("section_expansion") {
        Mustache::Mustache<std::string> tmpl{"<{{#lambda}}-{{/lambda}}>"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string& text){
            return text + "{{planet}}" + text;
        }}});
        data["planet"] = "Earth";
        CHECK(tmpl.render(data) == "<-Earth->");
    }

    SECTION("section_alternate_delimiters") {
        Mustache::Mustache<std::string> tmpl{"{{= | | =}}<|#lambda|-|/lambda|>"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string& text){
            return text + "{{planet}} => |planet|" + text;
        }}});
        data["planet"] = "Earth";
        CHECK(tmpl.render(data) == "<-{{planet}} => Earth->");
    }

    SECTION("section_multiple_calls") {
        Mustache::Mustache<std::string> tmpl{"{{#lambda}}FILE{{/lambda}} != {{#lambda}}LINE{{/lambda}}"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string& text){
            return "__" + text + "__";
        }}});
        CHECK(tmpl.render(data) == "__FILE__ != __LINE__");
    }

    SECTION("section_inverted") {
        Mustache::Mustache<std::string> tmpl{"<{{^lambda}}{{static}}{{/lambda}}>"};
        Data data("lambda", Data{Data::LambdaType{[](const std::string& text){
            CHECK(text == "{{static}}");
            return Data::Type::False;
        }}});
        data["static"] = "static";
        CHECK(tmpl.render(data) == "<>");
    }

}

TEST_CASE("dotted_names") {
    
    using Data = Mustache::Data<std::string>;
    
    SECTION("basic") {
        Mustache::Mustache<std::string> tmpl{"\"{{person.name}}\" == \"{{#person}}{{name}}{{/person}}\""};
        Data person{"name", "Joe"};
        CHECK(tmpl.render({"person", person}) == "\"Joe\" == \"Joe\"");
    }

    SECTION("triple_mustache") {
        Mustache::Mustache<std::string> tmpl{"\"{{{person.name}}}\" == \"{{#person}}{{{name}}}{{{/person}}}\""};
        Data person{"name", "Joe"};
        CHECK(tmpl.render({"person", person}) == "\"Joe\" == \"Joe\"");
    }

    SECTION("ampersand") {
        Mustache::Mustache<std::string> tmpl{"\"{{&person.name}}\" == \"{{#person}}{{&name}}{{{/person}}}\""};
        Data person{"name", "Joe"};
        CHECK(tmpl.render({"person", person}) == "\"Joe\" == \"Joe\"");
    }

    SECTION("depth") {
        Mustache::Mustache<std::string> tmpl{"\"{{a.b.c.d.e.name}}\" == \"Phil\""};
        Data data{"a", {"b", {"c", {"d", {"e", {"name", "Phil"}}}}}};
        CHECK(tmpl.render(data) == "\"Phil\" == \"Phil\"");
    }

    SECTION("broken_chains1") {
        Mustache::Mustache<std::string> tmpl{"\"{{a.b.c}}\" == \"\""};
        Data data{"a", Data::List()};
        CHECK(tmpl.render(data) == "\"\" == \"\"");
    }

    SECTION("broken_chains2") {
        Mustache::Mustache<std::string> tmpl{"\"{{a.b.c.name}}\" == \"\""};
        Data data;
        data["a"] = {"b", Data::List()};
        data["c"] = {"name", "Jim"};
        CHECK(tmpl.render(data) == "\"\" == \"\"");
    }

    SECTION("depth2") {
        Mustache::Mustache<std::string> tmpl{"\"{{#a}}{{b.c.d.e.name}}{{/a}}\" == \"Phil\""};
        Data data;
        data["a"] = {"b", {"c", {"d", {"e", {"name", "Phil"}}}}};
        data["b"] = {"c", {"d", {"e", {"name", "Wrong"}}}};
        CHECK(tmpl.render(data) == "\"Phil\" == \"Phil\"");
    }

    SECTION("scope") {
        Mustache::Mustache<std::string> tmpl{"\"{{#a}}{{b.name}}{{/a}}\" == \"Phil\""};
        Data data;
        data["a"] = {"x", "y"};
        data["b"] = {"name", "Phil"};
        CHECK(tmpl.render(data) == "\"Phil\" == \"Phil\"");
    }
}
