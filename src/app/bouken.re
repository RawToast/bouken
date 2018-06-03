module CreateGame: ((Types.GameLoop, Types.World, Types.WorldBuilder) => (Types.Game)) = 
  (GL: Types.GameLoop, W: Types.World, WB: Types.WorldBuilder) => {
    
  open Rationale;
  open Level;
  open Types;

  let initPlayer = name => {name: name, stats: { health: 10, speed: 1.0, position: 0. }, gold: 0, location: (6, 6)};

  let initgame = pname => initPlayer(pname) |> p => {
      player: p,
      world: WB.create(p),
      turn: 0.
      };

  let create = name => initgame(name);

  let calculateScore = game => {
    let baseScore = 1000;
    let turnPenalty = int_of_float((game.turn /. 5.));
    let goldBonus = game.player.gold * 10;
    let healthBonus = game.player.stats.health * 2;

    baseScore + healthBonus + goldBonus - turnPenalty;
  };

  let gameToRes = game => {
    if (game.player.stats.health > 0) Ok(game)
    else EndGame(calculateScore(game), game.player.name);
  };

  let attack = (x, y, game) => {
    open Rationale.Option;

    let level = W.currentLevel(game.world);
    let player = game.player;
    let (xl, yl) = player.location;
    let tx = x + xl;
    let ty = y + yl;

    let findPlaceToAttack = l => Area.getPlace(tx, ty, l.map);

    let attackPlace = p => switch(p.state) {
      | Enemy(e) => {
        let newEnemy = { ... e, stats: { ... e.stats, health: e.stats.health - 5 } };
        if (newEnemy.stats.health > 0) Some( {...p, state: Enemy(newEnemy) })
        else Some( { ... p, state: Empty })
      }
      | _ => None 
    };
    
    let reducedPlayer = { ... player, stats: { ... player.stats, position: player.stats.position -. 1. } };
      
    let updateLevelWithPlayer = level => {
      level.map 
        |> Area.setPlayerAt(xl, yl, reducedPlayer, 0.)
        |> Option.ofResult
        |> Option.fmap(area => { ... level, map: area });
    };

    let goldBonus (x, y, game) = { 

      let player = game.player;

      let updateCurrentLevel = area 
        => W.currentLevel(game.world) 
        |> Option.fmap(l => { ... l, map: area })
        |> Option.fmap(l => W.updateLevel(l, game.world));

      let applyBonus = gam => W.currentLevel(gam.world)
        >>= level => {
          let bonus = Area.getPlace(x, y, level.map)
            |> Option.fmap(p => switch(p.state) {
                | Empty => 5
                | _ => 0
                })
            |> Option.default(0);

          let updatedPlayer = { ... player, gold: player.gold + bonus };

          Area.setPlayerAt(xl, yl, updatedPlayer, 0., level.map) 
            |> Option.ofResult
            >>= updateCurrentLevel
            |> Option.fmap(w => { ... gam, world: w, player: updatedPlayer});
        };

      game |> applyBonus;
    };

    level 
      >>= findPlaceToAttack 
      >>= attackPlace
      >>= (p => level |> Option.fmap(Level.modifyTile(tx, ty, p)) )
      >>= updateLevelWithPlayer
      |> Option.fmap(l => World.World.updateLevel(l, game.world))
      |> Option.fmap(w => { ... game, world: w})
      >>= goldBonus(tx, ty)
      |> Option.fmap(gameToRes)
      |> Option.default(Error("Unable to attack"))
  };

  let movePlayer = (x, y, game) => {
    let level = W.currentLevel(game.world);
    let (xl, yl) = game.player.location;
    let canMove = level 
      |> Option.fmap(l => Area.canMoveTo(~overwrite=false, x + xl, y + yl, l.map) |> Result.isOk) 
      |> Option.default(false);
    let canAttack = level 
      |> Option.fmap(l => Area.canMoveTo(~overwrite=true, x + xl, y + yl, l.map) |> Result.isOk) 
      |> Option.default(false);

    let attemptMove(level, x, y) = level |> 
      Option.bind(_, l =>
          l.map |> Area.movePlayer(x, y, 1.) |>
              Option.ofResult |> o => switch o {
                  | Some(area) => Some(area)
                  | None => None
                  }
              |> Option.fmap((pa:playerArea) => {
                  let nl = { ... l, map: pa.area};
                  let world = W.updateLevel(nl, game.world);

                  {... game, player: pa.player, world: world };
              }))
      |> Option.fmap(GL.continue)
      |> optRes => switch optRes {
        | Some(game) => gameToRes(game)
        | None => Error("Unable to move player")
        };

    if (canMove) {
      attemptMove(level,x , y)
    } else if (canAttack) {
      attack(x, y, game)
    } else {
      Error("Unable to move player")
    };
  };

  let useStairs = game => {
    let (cx, cy) = game.player.location;
    let currentLevel = W.currentLevel(game.world) 
      |> Option.fmap(Level.removeOccupant(cx, cy));
    
    let currentLink: option(link) = currentLevel |>
      Option.bind(_, level => {
        let (x, y) = game.player.location;
        Area.getPlace(x, y, level.map)
          |> Option.bind(_, p => switch (p.tile) {
            | STAIRS(link) => Some(link)
            | _ => None
          });
      });

    let findLinkedLevel: link => option(level) = link => W.selectLevel(link.level, game.world);
    let stairs: (level, link) => option(place) = (lev, link) => Area.findStairs(link.id, lev.map);

    let findLinkLocation: option(link) => option(place) = optLink => Option.bind(optLink, link => {
      let newLevel: option(level) = findLinkedLevel(link);
      let levelStairs: option(place) = Option.bind(newLevel, lev => stairs(lev, link));
      levelStairs;
    });

    let coord: (option(level), option(place)) => option((int, int)) = (optL, optP) => 
      Option.bind(optL, level => {
        Option.bind(optP, place => {
          switch place.tile {
            | STAIRS(link) => Area.locationOfStairs(link.id, level.map)
            | _ => None
            }
        })
    });
    
    let linkedLevel = Option.bind(currentLink, findLinkedLevel);
    let linkedPlace = findLinkLocation(currentLink);

    let positionOpt = coord(linkedLevel, linkedPlace);

    Option.bind(currentLevel, prevLevel => {
      Option.bind(positionOpt, pos => {
        let (x, y) = pos;
        linkedLevel |> Option.bind(_, l => {
          let world = game.world;
          let current = l.name;
          let pre = {
            ... game, 
            player: {... game.player, location: (x, y), stats: {... game.player.stats, position: game.player.stats.position -. 1.5 }},
            world: { levels: world.levels, current: current } 
          };

          let upAreaOpt = Area.setPlayerAt(x, y, pre.player, 1., l.map) |> Rationale.Result.getOk;
          let upArea = upAreaOpt |> Option.default(l.map);
          let updatedLevel = {...l, map: upArea};

          let nextWorld = pre.world |> W.updateLevel(prevLevel) |> W.updateLevel(updatedLevel);

          Some({ ... pre, world: nextWorld});
        });
      });
    }) 
    |> Option.fmap(GL.continue)
    |> optRes => switch optRes {
      | Some(game) => Ok(game)
      | None => Error("Unable to use stairs")
    }
  };

  let useExit = game => {
    let currentLevel = W.currentLevel(game.world);

    let currentExitScore: option(int) = currentLevel |>
      Option.bind(_, level => {
        let (x, y) = game.player.location;
        Area.getPlace(x, y, level.map)
          |> Option.bind(_, p => switch (p.tile) {
            | EXIT(score) => Some(score)
            | _ => None
          });
      });

    let baseBonus = 1000;

    switch(currentExitScore) {
    | Some(score) => EndGame((score + baseBonus + calculateScore(game)), game.player.name);
    | None => Ok(game)
    }
  };
};
