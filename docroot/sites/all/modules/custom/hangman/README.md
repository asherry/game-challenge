Overview:
- choose word
- response, correct / wrong
- response, correct / wrong
...etc


Architecture:
game, game instance, game session
- player id
- status (incomplete, active, complete)
- created
- updated
  bundles: hangman
  - word
  - guesses
  - score

Configuration
- word list
- max guesses 
- bonus questions
- scoring logic 

player
- email
- name
