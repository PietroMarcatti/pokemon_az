import sys
import asyncio
from poke_env import RandomPlayer, Player
from poke_env.data import GenData
import torch
import torch.nn as nn
import torch.nn.functional as F

class PokemonAZNet(nn.Module):
    def __init__(self, input_dim, output_dim):
        super(PokemonAZNet, self).__init__()
        self.fc1 = nn.Linear(input_dim, 256)
        self.fc2 = nn.Linear(256, 256)
        self.fc3 = nn.Linear(256, 256)
        self.policy_head = nn.Linear(256, output_dim)
        self.value_head = nn.Linear(256, 1)

    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        policy_logits = self.policy_head(x)
        value = torch.tanh(self.value_head(x))
        return policy_logits, value

class AlphaZeroPlayer(Player):
    def __init__(self, account_configuration = None, *, avatar = None, battle_format = "gen9randombattle", log_level = None, max_concurrent_battles = 1, accept_open_team_sheet = False, save_replays = False, server_configuration = None, start_timer_on_battle_start = False, start_listening = True, open_timeout = 10, ping_interval = 20, ping_timeout = 20, team = None):
        super().__init__(account_configuration, avatar=avatar, battle_format=battle_format, log_level=log_level, max_concurrent_battles=max_concurrent_battles, accept_open_team_sheet=accept_open_team_sheet, save_replays=save_replays, server_configuration=server_configuration, start_timer_on_battle_start=start_timer_on_battle_start, start_listening=start_listening, open_timeout=open_timeout, ping_interval=ping_interval, ping_timeout=ping_timeout, team=team)
        self.model = PokemonAZNet(1372,11)
        ts_model = torch.jit.load("/home/pietro/repos/pokemon_az/out/build/linux-debug/checkpoints/best.pt", map_location='cpu')

        # Extract state dict from TorchScript module
        state_dict = {k: v for k, v in ts_model.state_dict().items()}

        # Load into your model
        self.model.load_state_dict(state_dict)
        self.model.eval()
    def choose_move(self, battle):
        self.choose_random_move(battle)

async def main():
    random_player = RandomPlayer(battle_format="gen1randombattle")
    second_player = RandomPlayer(battle_format="gen1randombattle")
    test_player = AlphaZeroPlayer()

    await random_player.battle_against(second_player, n_battles=50)

    print(
        f"Player {random_player.username} won {random_player.n_won_battles} out of {random_player.n_finished_battles} played"
    )
    print(
        f"Player {second_player.username} won {second_player.n_won_battles} out of {second_player.n_finished_battles} played"
    )

    for battle_tag, battle in random_player.battles.items():
        print(battle_tag, battle.won)

# Run the async main function
if __name__ == "__main__":
    asyncio.run(main())
