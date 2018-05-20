module CreateGame: ((Types.GameLoop, Types.World) => (Types.Game)) = (GL: Types.GameLoop, W: Types.World) => {
    
  open Rationale;
  open Level;
  open Types;

  let initPlayer = name => {name: name, stats: { health: 10, speed: 1.0, position: 0. }, gold: 5, location: (6, 6)};

  let initgame = pname => initPlayer(pname) |> p => {
      player: p,
      world: W.create(p),
      turn: 0.
      };

  let create = name => initgame(name);

  let movePlayer = (x, y, game) => {
    let level = W.currentLevel(game.world);

    level |> 
      Option.bind(_, l =>
          l.map |> Area.movePlayer(x, y, 1.) |>
              Option.ofResult |> o => switch o {
                  | Some(area) => Some(area)
                  | None => None
                  }
              |> Option.fmap((pa:playerArea) => {
                  let nl = { ... l, map: pa.area};
                  let world = W.updateLevel(nl, game.world);

                  let (posx, posy) = pa.player.location;
                  {... game, player: pa.player, world: world };
              }))
      |> Option.fmap(GL.continue);
  };

  let useStairs = game => {
    let levelOpt = W.currentLevel(game.world);
    let (x, y) = game.player.location;
    
    let currentLink: option(link) = levelOpt |>
      Option.bind(_, level => {
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

    let maybeLink = place => switch place.tile {
      | STAIRS(link) => Some(link)
      | _ => None
      };

    let coord: (option(level), option(place)) => option((int, int)) = (optL, optP) => 
      Option.bind(optL, level => {
        Option.bind(optP, place => {
          let optLoc = l => Option.bind(l, link => Area.locationOfStairs(link.id, level.map));

          maybeLink(place) |> optLoc;
        })
    });
    
    let linkedLevel = Option.bind(currentLink, l => findLinkedLevel(l));
    let linkedPlace = findLinkLocation(currentLink);

    let positionOpt = coord(linkedLevel, linkedPlace);

    let (cx, cy) = game.player.location;
    let newPrevLevel = levelOpt |> Option.fmap(Level.removeOccupant(cx, cy));

    Option.bind(newPrevLevel, prevLevel => {
      Option.bind(positionOpt, pos => {
        let (x, y) = pos;
        linkedLevel |> Option.bind(_, l => {
          let world = game.world;
          let current = l.name;
          let pre = {
            ... game, 
            player: {... game.player, location: (x, y)},
            world: { levels: world.levels, current: current } 
          };

          let upAreaOpt = Area.setPlayerAt(x, y, pre.player, 1., l.map) |> Rationale.Result.getOk;
          let upArea = upAreaOpt |> Option.default(l.map);
          let updatedLevel = {...l, map: upArea};

          let nextWorld = pre.world |> W.updateLevel(prevLevel) |> W.updateLevel(updatedLevel);

          Some({ ... pre, world: nextWorld});
        });
      });
    });
  };
};