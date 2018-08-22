# Bouken

ASCII rogue-like 🙀

[![Build Status](https://travis-ci.org/RawToast/bouken.svg?branch=master)](https://travis-ci.org/RawToast/bouken)
[![Coverage Status](https://coveralls.io/repos/github/RawToast/bouken/badge.svg?branch=master)](https://coveralls.io/github/RawToast/bouken?branch=master)

## Controls

The player can move in 8 directions with:

 `q` `w` `e`

 `a` 🐾 `d`

 `z` `x` `c`

The player can attack by moving in the direction of an enemy.

Stairs `/` can be taken in either direction using `s`.

The player can win the game using an exit `e` using `s`, but remember some exits score more points than others.

`s` will wait a turn if the player is not on stairs or an exit.

## Key

`O` Player

`.` Ground

`:` Tough terrain (Slower movement/attack speed)

`w` Shallow water (Very slow movement/attack speed)

`#` Wall

`Z` Enemy / Zombie

`X` Enemy / Generic Enemy

`M` Enemy / Minotaur

`+` Health

`g` Gold

`,` Trap

`;` Snare

`/` Stairs

`e` Exit

## Deployment

* Install bs-platform `npm install -g bs-platform@4.0.3`
* Install dependencies `yarn`
* Compile reason `yarn build`
* Run tests `yarn test`
* Test coverage `yarn coverage`
* Development server with watch mode `yarn start`

## Docker

The provided dockerfile can be used to deploy the app using [Now](https://zeit.co/now)

`now --no-verify --public --docker`

## Stuff

* [ReasonML](https://reasonml.github.io/)
* [Reason React](https://reasonml.github.io/reason-react/)
* [Rationale](https://github.com/jonlaing/rationale)
* [BS Jest](https://github.com/glennsl/bs-jest)
