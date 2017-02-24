#include <verbly.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <twitter.h>
#include "sentence.h"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "usage: support [configfile]" << std::endl;
    return -1;
  }

  std::string configfile(argv[1]);
  YAML::Node config = YAML::LoadFile(configfile);

  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());

  twitter::client client(auth);

  std::random_device randomDevice;
  std::mt19937 rng{randomDevice()};

  verbly::database database(config["verbly_datafile"].as<std::string>());
  sentence generator(database, rng);

  for (;;)
  {
    verbly::word adjective = database.words(
      (verbly::notion::partOfSpeech == verbly::part_of_speech::adjective)
      && (verbly::word::antiPertainyms %=
        (verbly::word::forms(verbly::inflection::plural)))).first();

    verbly::word noun = database.words(
      (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
      && (verbly::word::pertainyms %= adjective)
      && (verbly::word::forms(verbly::inflection::plural))).first();

    verbly::token action = {
      "RT if you ARE",
      verbly::token::punctuation(",", adjective),
      "if you SUPPORT",
      verbly::token::punctuation(",",
        verbly::token(noun, verbly::inflection::plural)),
      "or if you",
      generator.generate()};

    std::string result = action.compile();
    if (result.length() <= 140)
    {
      std::cout << result << std::endl;

      try
      {
        client.updateStatus(result);

        std::cout << "Tweeted!" << std::endl;
      } catch (const twitter::twitter_error& e)
      {
        std::cout << "Twitter error: " << e.what() << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::hours(1));

      std::cout << std::endl;
    }
  }
}

