( define ( problem probname )
  ( :domain depots )
  ( :requirements :strips :typing :equality )

  ( :objects
    place000 - place
    place001 - place
    place002 - place
    place003 - place
    place004 - place
    place005 - place
    place006 - place
    place007 - place
    truck000 - truck
    truck001 - truck
    truck002 - truck
    truck003 - truck
    truck004 - truck
    truck005 - truck
    pallet000 - surface
    pallet001 - surface
    pallet002 - surface
    pallet003 - surface
    pallet004 - surface
    pallet005 - surface
    pallet006 - surface
    pallet007 - surface
    pallet008 - surface
    pallet009 - surface
    pallet010 - surface
    pallet011 - surface
    pallet012 - surface
    pallet013 - surface
    pallet014 - surface
    pallet015 - surface
    crate063 - surface
    crate027 - surface
    crate035 - surface
    crate044 - surface
    crate065 - surface
    crate043 - surface
    crate062 - surface
    crate013 - surface
    crate002 - surface
    crate059 - surface
    crate036 - surface
    crate005 - surface
    crate033 - surface
    crate037 - surface
    crate008 - surface
    crate000 - surface
    crate012 - surface
    crate025 - surface
    crate053 - surface
    crate066 - surface
    crate030 - surface
    crate079 - surface
    crate006 - surface
    crate029 - surface
    crate054 - surface
    crate060 - surface
    crate041 - surface
    crate052 - surface
    crate064 - surface
    crate010 - surface
    crate019 - surface
    crate011 - surface
    crate047 - surface
    crate068 - surface
    crate074 - surface
    crate032 - surface
    crate057 - surface
    crate040 - surface
    crate022 - surface
    crate028 - surface
    crate071 - surface
    crate018 - surface
    crate039 - surface
    crate026 - surface
    crate077 - surface
    crate056 - surface
    crate045 - surface
    crate078 - surface
    crate014 - surface
    crate021 - surface
    crate055 - surface
    crate048 - surface
    crate046 - surface
    crate042 - surface
    crate051 - surface
    crate061 - surface
    crate073 - surface
    crate007 - surface
    crate003 - surface
    crate031 - surface
    crate075 - surface
    crate038 - surface
    crate016 - surface
    crate009 - surface
    crate015 - surface
    crate049 - surface
    crate070 - surface
    crate067 - surface
    crate020 - surface
    crate058 - surface
    crate023 - surface
    crate034 - surface
    crate050 - surface
    crate076 - surface
    crate017 - surface
    crate069 - surface
    crate072 - surface
    crate024 - surface
    crate001 - surface
    crate004 - surface
    hoist000 - hoist
    hoist001 - hoist
    hoist002 - hoist
    hoist003 - hoist
    hoist004 - hoist
    hoist005 - hoist
    hoist006 - hoist
    hoist007 - hoist
  )

  ( :init
    ( truck-at truck000 place003 )
    ( truck-at truck001 place004 )
    ( truck-at truck002 place006 )
    ( truck-at truck003 place000 )
    ( truck-at truck004 place001 )
    ( truck-at truck005 place003 )
    ( hoist-at hoist000 place000 )
    ( hoist-at hoist001 place001 )
    ( hoist-at hoist002 place002 )
    ( hoist-at hoist003 place003 )
    ( hoist-at hoist004 place004 )
    ( hoist-at hoist005 place005 )
    ( hoist-at hoist006 place006 )
    ( hoist-at hoist007 place007 )
    ( available hoist000 )
    ( available hoist001 )
    ( available hoist002 )
    ( available hoist003 )
    ( available hoist004 )
    ( available hoist005 )
    ( available hoist006 )
    ( available hoist007 )
    ( surface-at pallet000 place000 )
    ( surface-at pallet001 place001 )
    ( surface-at pallet002 place002 )
    ( surface-at pallet003 place003 )
    ( surface-at pallet004 place004 )
    ( surface-at pallet005 place005 )
    ( surface-at pallet006 place006 )
    ( surface-at pallet007 place007 )
    ( surface-at pallet008 place002 )
    ( surface-at pallet009 place002 )
    ( surface-at pallet010 place006 )
    ( surface-at pallet011 place006 )
    ( surface-at pallet012 place007 )
    ( surface-at pallet013 place004 )
    ( surface-at pallet014 place001 )
    ( surface-at pallet015 place007 )
    ( surface-at crate063 place000 )
    ( surface-at crate027 place000 )
    ( surface-at crate035 place000 )
    ( surface-at crate044 place001 )
    ( surface-at crate065 place001 )
    ( surface-at crate043 place001 )
    ( surface-at crate062 place001 )
    ( surface-at crate013 place002 )
    ( surface-at crate002 place002 )
    ( surface-at crate059 place002 )
    ( surface-at crate036 place003 )
    ( surface-at crate005 place003 )
    ( surface-at crate033 place003 )
    ( surface-at crate037 place003 )
    ( surface-at crate008 place003 )
    ( surface-at crate000 place004 )
    ( surface-at crate012 place004 )
    ( surface-at crate025 place004 )
    ( surface-at crate053 place004 )
    ( surface-at crate066 place004 )
    ( surface-at crate030 place004 )
    ( surface-at crate079 place005 )
    ( surface-at crate006 place005 )
    ( surface-at crate029 place005 )
    ( surface-at crate054 place005 )
    ( surface-at crate060 place005 )
    ( surface-at crate041 place006 )
    ( surface-at crate052 place006 )
    ( surface-at crate064 place006 )
    ( surface-at crate010 place006 )
    ( surface-at crate019 place006 )
    ( surface-at crate011 place006 )
    ( surface-at crate047 place007 )
    ( surface-at crate068 place007 )
    ( surface-at crate074 place007 )
    ( surface-at crate032 place007 )
    ( surface-at crate057 place007 )
    ( surface-at crate040 place007 )
    ( surface-at crate022 place007 )
    ( surface-at crate028 place007 )
    ( surface-at crate071 place007 )
    ( surface-at crate018 place007 )
    ( surface-at crate039 place007 )
    ( surface-at crate026 place007 )
    ( surface-at crate077 place002 )
    ( surface-at crate056 place002 )
    ( surface-at crate045 place002 )
    ( surface-at crate078 place002 )
    ( surface-at crate014 place002 )
    ( surface-at crate021 place002 )
    ( surface-at crate055 place002 )
    ( surface-at crate048 place002 )
    ( surface-at crate046 place002 )
    ( surface-at crate042 place006 )
    ( surface-at crate051 place006 )
    ( surface-at crate061 place006 )
    ( surface-at crate073 place006 )
    ( surface-at crate007 place006 )
    ( surface-at crate003 place006 )
    ( surface-at crate031 place006 )
    ( surface-at crate075 place006 )
    ( surface-at crate038 place006 )
    ( surface-at crate016 place006 )
    ( surface-at crate009 place006 )
    ( surface-at crate015 place007 )
    ( surface-at crate049 place004 )
    ( surface-at crate070 place004 )
    ( surface-at crate067 place004 )
    ( surface-at crate020 place004 )
    ( surface-at crate058 place001 )
    ( surface-at crate023 place001 )
    ( surface-at crate034 place001 )
    ( surface-at crate050 place001 )
    ( surface-at crate076 place001 )
    ( surface-at crate017 place001 )
    ( surface-at crate069 place001 )
    ( surface-at crate072 place001 )
    ( surface-at crate024 place007 )
    ( surface-at crate001 place007 )
    ( surface-at crate004 place007 )
    ( is-crate crate063 )
    ( is-crate crate027 )
    ( is-crate crate035 )
    ( is-crate crate044 )
    ( is-crate crate065 )
    ( is-crate crate043 )
    ( is-crate crate062 )
    ( is-crate crate013 )
    ( is-crate crate002 )
    ( is-crate crate059 )
    ( is-crate crate036 )
    ( is-crate crate005 )
    ( is-crate crate033 )
    ( is-crate crate037 )
    ( is-crate crate008 )
    ( is-crate crate000 )
    ( is-crate crate012 )
    ( is-crate crate025 )
    ( is-crate crate053 )
    ( is-crate crate066 )
    ( is-crate crate030 )
    ( is-crate crate079 )
    ( is-crate crate006 )
    ( is-crate crate029 )
    ( is-crate crate054 )
    ( is-crate crate060 )
    ( is-crate crate041 )
    ( is-crate crate052 )
    ( is-crate crate064 )
    ( is-crate crate010 )
    ( is-crate crate019 )
    ( is-crate crate011 )
    ( is-crate crate047 )
    ( is-crate crate068 )
    ( is-crate crate074 )
    ( is-crate crate032 )
    ( is-crate crate057 )
    ( is-crate crate040 )
    ( is-crate crate022 )
    ( is-crate crate028 )
    ( is-crate crate071 )
    ( is-crate crate018 )
    ( is-crate crate039 )
    ( is-crate crate026 )
    ( is-crate crate077 )
    ( is-crate crate056 )
    ( is-crate crate045 )
    ( is-crate crate078 )
    ( is-crate crate014 )
    ( is-crate crate021 )
    ( is-crate crate055 )
    ( is-crate crate048 )
    ( is-crate crate046 )
    ( is-crate crate042 )
    ( is-crate crate051 )
    ( is-crate crate061 )
    ( is-crate crate073 )
    ( is-crate crate007 )
    ( is-crate crate003 )
    ( is-crate crate031 )
    ( is-crate crate075 )
    ( is-crate crate038 )
    ( is-crate crate016 )
    ( is-crate crate009 )
    ( is-crate crate015 )
    ( is-crate crate049 )
    ( is-crate crate070 )
    ( is-crate crate067 )
    ( is-crate crate020 )
    ( is-crate crate058 )
    ( is-crate crate023 )
    ( is-crate crate034 )
    ( is-crate crate050 )
    ( is-crate crate076 )
    ( is-crate crate017 )
    ( is-crate crate069 )
    ( is-crate crate072 )
    ( is-crate crate024 )
    ( is-crate crate001 )
    ( is-crate crate004 )
    ( on crate063 pallet000 )
    ( on crate027 crate063 )
    ( on crate035 crate027 )
    ( clear crate035 )
    ( on crate044 pallet001 )
    ( on crate065 crate044 )
    ( on crate043 crate065 )
    ( on crate062 crate043 )
    ( clear crate062 )
    ( on crate013 pallet002 )
    ( on crate002 crate013 )
    ( on crate059 crate002 )
    ( clear crate059 )
    ( on crate036 pallet003 )
    ( on crate005 crate036 )
    ( on crate033 crate005 )
    ( on crate037 crate033 )
    ( on crate008 crate037 )
    ( clear crate008 )
    ( on crate000 pallet004 )
    ( on crate012 crate000 )
    ( on crate025 crate012 )
    ( on crate053 crate025 )
    ( on crate066 crate053 )
    ( on crate030 crate066 )
    ( clear crate030 )
    ( on crate079 pallet005 )
    ( on crate006 crate079 )
    ( on crate029 crate006 )
    ( on crate054 crate029 )
    ( on crate060 crate054 )
    ( clear crate060 )
    ( on crate041 pallet006 )
    ( on crate052 crate041 )
    ( on crate064 crate052 )
    ( on crate010 crate064 )
    ( on crate019 crate010 )
    ( on crate011 crate019 )
    ( clear crate011 )
    ( on crate047 pallet007 )
    ( on crate068 crate047 )
    ( on crate074 crate068 )
    ( on crate032 crate074 )
    ( on crate057 crate032 )
    ( on crate040 crate057 )
    ( on crate022 crate040 )
    ( on crate028 crate022 )
    ( on crate071 crate028 )
    ( on crate018 crate071 )
    ( on crate039 crate018 )
    ( on crate026 crate039 )
    ( clear crate026 )
    ( on crate077 pallet008 )
    ( on crate056 crate077 )
    ( on crate045 crate056 )
    ( on crate078 crate045 )
    ( on crate014 crate078 )
    ( clear crate014 )
    ( on crate021 pallet009 )
    ( on crate055 crate021 )
    ( on crate048 crate055 )
    ( on crate046 crate048 )
    ( clear crate046 )
    ( on crate042 pallet010 )
    ( on crate051 crate042 )
    ( on crate061 crate051 )
    ( on crate073 crate061 )
    ( on crate007 crate073 )
    ( clear crate007 )
    ( on crate003 pallet011 )
    ( on crate031 crate003 )
    ( on crate075 crate031 )
    ( on crate038 crate075 )
    ( on crate016 crate038 )
    ( on crate009 crate016 )
    ( clear crate009 )
    ( on crate015 pallet012 )
    ( clear crate015 )
    ( on crate049 pallet013 )
    ( on crate070 crate049 )
    ( on crate067 crate070 )
    ( on crate020 crate067 )
    ( clear crate020 )
    ( on crate058 pallet014 )
    ( on crate023 crate058 )
    ( on crate034 crate023 )
    ( on crate050 crate034 )
    ( on crate076 crate050 )
    ( on crate017 crate076 )
    ( on crate069 crate017 )
    ( on crate072 crate069 )
    ( clear crate072 )
    ( on crate024 pallet015 )
    ( on crate001 crate024 )
    ( on crate004 crate001 )
    ( clear crate004 )
  )
  ( :goal
    ( and
      ( on crate005 pallet000 )
      ( on crate043 crate005 )
      ( on crate000 crate043 )
      ( on crate006 crate000 )
      ( on crate001 pallet001 )
      ( on crate019 crate001 )
      ( on crate069 crate019 )
      ( on crate035 pallet002 )
      ( on crate003 crate035 )
      ( on crate037 crate003 )
      ( on crate077 crate037 )
      ( on crate068 crate077 )
      ( on crate059 crate068 )
      ( on crate039 pallet003 )
      ( on crate029 crate039 )
      ( on crate052 crate029 )
      ( on crate050 crate052 )
      ( on crate024 crate050 )
      ( on crate057 crate024 )
      ( on crate053 crate057 )
      ( on crate079 crate053 )
      ( on crate041 pallet004 )
      ( on crate016 crate041 )
      ( on crate046 crate016 )
      ( on crate031 crate046 )
      ( on crate061 crate031 )
      ( on crate063 crate061 )
      ( on crate067 pallet005 )
      ( on crate071 crate067 )
      ( on crate078 crate071 )
      ( on crate021 crate078 )
      ( on crate030 crate021 )
      ( on crate009 pallet006 )
      ( on crate051 crate009 )
      ( on crate056 crate051 )
      ( on crate065 crate056 )
      ( on crate042 crate065 )
      ( on crate075 crate042 )
      ( on crate014 pallet007 )
      ( on crate012 crate014 )
      ( on crate002 crate012 )
      ( on crate070 pallet008 )
      ( on crate049 crate070 )
      ( on crate022 crate049 )
      ( on crate054 crate022 )
      ( on crate040 pallet009 )
      ( on crate017 crate040 )
      ( on crate010 crate017 )
      ( on crate025 crate010 )
      ( on crate033 crate025 )
      ( on crate034 crate033 )
      ( on crate062 crate034 )
      ( on crate027 crate062 )
      ( on crate023 crate027 )
      ( on crate032 crate023 )
      ( on crate072 crate032 )
      ( on crate007 pallet010 )
      ( on crate048 crate007 )
      ( on crate008 crate048 )
      ( on crate013 crate008 )
      ( on crate066 crate013 )
      ( on crate060 pallet011 )
      ( on crate058 crate060 )
      ( on crate076 crate058 )
      ( on crate064 crate076 )
      ( on crate073 pallet012 )
      ( on crate044 crate073 )
      ( on crate055 crate044 )
      ( on crate038 crate055 )
      ( on crate074 crate038 )
      ( on crate015 crate074 )
      ( on crate028 crate015 )
      ( on crate020 crate028 )
      ( on crate011 pallet013 )
      ( on crate004 crate011 )
      ( on crate047 pallet014 )
      ( on crate045 crate047 )
      ( on crate036 crate045 )
      ( on crate018 pallet015 )
      ( on crate026 crate018 )
    )
  )
)

