( define ( htn-problem probname )
  ( :domain depots )
  ( :requirements :strips :htn :typing :equality )
  ( :objects
    place000 - place
    place001 - place
    place002 - place
    place003 - place
    truck000 - truck
    truck001 - truck
    truck002 - truck
    truck003 - truck
    pallet000 - surface
    pallet001 - surface
    pallet002 - surface
    pallet003 - surface
    pallet004 - surface
    pallet005 - surface
    pallet006 - surface
    pallet007 - surface
    crate009 - surface
    crate012 - surface
    crate008 - surface
    crate018 - surface
    crate000 - surface
    crate002 - surface
    crate017 - surface
    crate014 - surface
    crate005 - surface
    crate001 - surface
    crate019 - surface
    crate010 - surface
    crate003 - surface
    crate013 - surface
    crate016 - surface
    crate006 - surface
    crate007 - surface
    crate015 - surface
    crate004 - surface
    crate011 - surface
    hoist000 - hoist
    hoist001 - hoist
    hoist002 - hoist
    hoist003 - hoist
  )

  ( :init
    ( truck-at truck000 place002 )
    ( truck-at truck001 place000 )
    ( truck-at truck002 place003 )
    ( truck-at truck003 place000 )
    ( hoist-at hoist000 place000 )
    ( hoist-at hoist001 place001 )
    ( hoist-at hoist002 place002 )
    ( hoist-at hoist003 place003 )
    ( available hoist000 )
    ( available hoist001 )
    ( available hoist002 )
    ( available hoist003 )
    ( surface-at pallet000 place000 )
    ( surface-at pallet001 place001 )
    ( surface-at pallet002 place002 )
    ( surface-at pallet003 place003 )
    ( surface-at pallet004 place003 )
    ( surface-at pallet005 place001 )
    ( surface-at pallet006 place001 )
    ( surface-at pallet007 place003 )
    ( surface-at crate009 place000 )
    ( surface-at crate012 place000 )
    ( surface-at crate008 place001 )
    ( surface-at crate018 place002 )
    ( surface-at crate000 place003 )
    ( surface-at crate002 place003 )
    ( surface-at crate017 place003 )
    ( surface-at crate014 place003 )
    ( surface-at crate005 place003 )
    ( surface-at crate001 place003 )
    ( surface-at crate019 place001 )
    ( surface-at crate010 place001 )
    ( surface-at crate003 place001 )
    ( surface-at crate013 place001 )
    ( surface-at crate016 place001 )
    ( surface-at crate006 place001 )
    ( surface-at crate007 place003 )
    ( surface-at crate015 place003 )
    ( surface-at crate004 place003 )
    ( surface-at crate011 place003 )
    ( is-crate crate009 )
    ( is-crate crate012 )
    ( is-crate crate008 )
    ( is-crate crate018 )
    ( is-crate crate000 )
    ( is-crate crate002 )
    ( is-crate crate017 )
    ( is-crate crate014 )
    ( is-crate crate005 )
    ( is-crate crate001 )
    ( is-crate crate019 )
    ( is-crate crate010 )
    ( is-crate crate003 )
    ( is-crate crate013 )
    ( is-crate crate016 )
    ( is-crate crate006 )
    ( is-crate crate007 )
    ( is-crate crate015 )
    ( is-crate crate004 )
    ( is-crate crate011 )
    ( on crate009 pallet000 )
    ( on crate012 crate009 )
    ( clear crate012 )
    ( on crate008 pallet001 )
    ( clear crate008 )
    ( on crate018 pallet002 )
    ( clear crate018 )
    ( on crate000 pallet003 )
    ( on crate002 crate000 )
    ( on crate017 crate002 )
    ( clear crate017 )
    ( on crate014 pallet004 )
    ( on crate005 crate014 )
    ( on crate001 crate005 )
    ( clear crate001 )
    ( on crate019 pallet005 )
    ( on crate010 crate019 )
    ( on crate003 crate010 )
    ( on crate013 crate003 )
    ( clear crate013 )
    ( on crate016 pallet006 )
    ( on crate006 crate016 )
    ( clear crate006 )
    ( on crate007 pallet007 )
    ( on crate015 crate007 )
    ( on crate004 crate015 )
    ( on crate011 crate004 )
    ( clear crate011 )
  )
  ( :tasks
    ( make-on crate007 pallet000 )
    ( make-on crate008 crate007 )
    ( make-on crate001 pallet001 )
    ( make-on crate003 pallet002 )
    ( make-on crate006 crate003 )
    ( make-on crate019 crate006 )
    ( make-on crate000 pallet003 )
    ( make-on crate002 pallet004 )
    ( make-on crate017 crate002 )
    ( make-on crate018 crate017 )
    ( make-on crate005 pallet005 )
    ( make-on crate012 crate005 )
    ( make-on crate014 pallet006 )
    ( make-on crate009 crate014 )
    ( make-on crate004 crate009 )
    ( make-on crate010 crate004 )
    ( make-on crate015 crate010 )
    ( make-on crate013 pallet007 )
    ( make-on crate011 crate013 )
    ( make-on crate016 crate011 )
  )
)

