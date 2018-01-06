# DistrOnMemKVStore
A distributed in-memory key-value store with active replication. Refactored from a school project.

# Instructions
To execute the program:

1 - ./table-server (n_lists) (secondaryport) -> initialize backup server
2 - ./table-server (n_lists) (primaryport) (ipsecondary) (secondaryport) -> initialize main server
3 - ./table-client (ipprimary:primaryport) (ipsecondary:secondaryport) -> client

# Instruções
Para executar o programa deve-se executar os seguinte passos:

1 - ./table-server (n_lists) (porto do secundário) -> este é o servidor secundário
2 - ./table-server (n_lists) (porto do primário) (porto do secundário) -> este é o primário
3 - ./table-client (ip:porto_primario) (ip:porto_secundario) -> este é o cliente