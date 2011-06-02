-- Configuration: 
--   conf_count = 5
--   conf_numeric_keys = 1
--   conf_variable_keys = 1
--   conf_max_keylength = 128
--   conf_variable_data = 1
--   conf_max_datalength = 1024
CREATE (NUMERIC_KEY)
INSERT (0, "64", 31)
INSERT (0, "70", 51)
INSERT (0, "93", 957)
INSERT (0, "53", 60)
INSERT (0, "68", 7)
ERASE  (0, "64")
ERASE  (0, "70")
ERASE  (0, "93")
ERASE  (0, "53")
ERASE  (0, "68")
FULLCHECK
CLOSE
