# truco

## Before playing

If you're using windows, you should execute the following to allow support for the card symbols. This will mantain compatibility between program set of symbols and your console (and keep the game playable).

```cmd
chcp 65001
```

This is a temporary change, don't worry. This will change you set of characters to **UTF-8**.

## TODO

- [X] Show friendly messages for both the entire game and each match
- [X] Redesign control flow to support other functionalities
- [X] Add other truco functionalities (e.g. ask for truco, hide card)
- [X] Create distinct and sharable interfaces for truco state and MCTS nodes
- [ ] Fix other minor bugs
- [ ] Create CPU "AI" (something better than random decision)


### TODO Details

- [ ] ReCreate state manager to control player to move, tentos, truco asking and others to allow control and copying of the state for futurre ismcts functions
