( define ( htn-problem probname )
  ( :domain depots )
  ( :requirements :strips :htn :typing :equality )
  ( :objects
    place000 - place
    place001 - place
    truck000 - truck
    truck001 - truck
    pallet000 - surface
    pallet001 - surface
    pallet002 - surface
    pallet003 - surface
    crate004 - surface
    crate002 - surface
    crate000 - surface
    crate003 - surface
    crate001 - surface
    hoist000 - hoist
    hoist001 - hoist
  )

  ( :init
    ( truck-at truck000 place001 )
    ( truck-at truck001 place001 )
    ( hoist-at hoist000 place000 )
    ( hoist-at hoist001 place001 )
    ( available hoist000 )
    ( available hoist001 )
    ( surface-at pallet000 place000 )
    ( surface-at pallet001 place001 )
    ( surface-at pallet002 place001 )
    ( surface-at pallet003 place000 )
    ( surface-at crate004 place000 )
    ( surface-at crate002 place000 )
    ( surface-at crate000 place001 )
    ( surface-at crate003 place000 )
    ( surface-at crate001 place000 )
    ( is-crate crate004 )
    ( is-crate crate002 )
    ( is-crate crate000 )
    ( is-crate crate003 )
    ( is-crate crate001 )
    ( on crate004 pallet000 )
    ( on crate002 crate004 )
    ( clear crate002 )
    ( on crate000 pallet001 )
    ( clear crate000 )
    ( clear pallet002 )
    ( on crate003 pallet003 )
    ( on crate001 crate003 )
    ( clear crate001 )
  )
  ( :tasks
    ( make-on crate000 pallet000 )
    ( make-on crate002 pallet002 )
    ( make-on crate004 pallet003 )
    ( make-on crate003 crate004 )
    ( make-on crate001 crate003 )
  )
)

