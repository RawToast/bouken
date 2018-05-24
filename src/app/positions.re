open Types;

module BasicPositions: Positions = {
  let divisor = 2.;
  let isActive = stats => stats.position >= 1.;
  let increment = stats => {...stats, position: stats.position +. (stats.speed /. divisor)};
  let incrementAll =
    List.map((ys: list(place)) =>
      ys
      |> List.map((place: place) =>
           switch (place.state) {
           | Empty => place
           | Player(p) => {...place, state: Player({...p, stats: increment(p.stats)})}
           | Enemy(e) => {...place, state: Enemy({...e, stats: increment(e.stats)})}
           }
         )
    );
};
