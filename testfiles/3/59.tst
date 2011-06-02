-- Configuration: 
--   conf_count = 5
--   conf_numeric_keys = 1
--   conf_variable_keys = 1
--   conf_max_keylength = 128
--   conf_variable_data = 1
--   conf_max_datalength = 1024
CREATE (NUMERIC_KEY)
INSERT (0, "58", 770)
INSERT (0, "38", 449)
INSERT (0, "71", 690)
INSERT (0, "69", 274)
INSERT (0, "28", 412)
ERASE  (0, "58")
ERASE  (0, "38")
ERASE  (0, "71")
ERASE  (0, "69")
ERASE  (0, "28")
FULLCHECK
CLOSE
