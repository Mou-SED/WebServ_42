import os
import sys
import random

x = os.getenv("QUERY_STRING")
x = x.split("=")[1]

def generate_choice():
  choices = ["rock", "scissors", "paper"]
  return random.choice(choices)

def play():
  user_choice = x
  computer_choice = generate_choice()

  result = ""

  if user_choice == computer_choice:
    result = "tie"
  elif user_choice == "rock":
    if computer_choice == "scissors":
      result = "win"
    else:
      result = "lose"
  elif user_choice == "scissors":
    if computer_choice == "paper":
      result = "win"
    else:
      result = "lose"
  else:
    if computer_choice == "rock":
      result = "win"
    else:
      result = "lose"

  # return "{\"result\": \"" + result + "\"}"
  return """{
    "result": \"""" + result + """\",
    "userChoice": \"""" + user_choice + """\",
    "computerChoice": \"""" + computer_choice + """\"
  }"""

if __name__ == "__main__":
  res = play()
  print(res)
