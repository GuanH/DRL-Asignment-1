from TaxiWrapper import MultiTaxiWrapper
import numpy as np
"""
I trained multiple runs with different epoch, eps, eps_rate1 and eps_rate2
and I didn't remember the actual recipe.
"""

EPOCH = 400000
FUEL = 200
alpha = 0.1
gamma = 0.99


def get_action_name(action):
    """Returns a human-readable action name."""
    actions = ["Move South", "Move North", "Move East",
               "Move West", "Pick Up", "Drop Off"]
    return actions[action] if action is not None else "None"


# q_table = np.zeros((19, 19, 2, 2, 2, 2, 2, 2, 2, 6))
q_table = np.fromfile('q_table.npz').reshape((19, 19, 2, 2, 2, 2, 2, 2, 2, 6))
eps = 0.2

eps_rate1 = (0.1/0.2)**(1/(EPOCH * 0.7))
eps_rate2 = (0.01/0.1)**(1/(EPOCH * 0.3))
taxi = MultiTaxiWrapper(1, 10, FUEL, 15)
reward_hist = []


for epoch in range(EPOCH):
    check = 0
    stage = 0
    drop = (-1, -1)
    tot_reward = 0
    taxi.reset()
    obs = taxi.get_state()[0]
    x, y = obs[0], obs[1]
    stations = [(obs[2+i*2], obs[2+i*2+1]) for i in range(4)]
    tx, ty = stations[check]

    s = [tx-x+9, ty-y+9, stage, obs[10], obs[11],
         obs[12], obs[13], obs[14], obs[15]]
    # taxi.render(0)
    for step in range(FUEL):
        obs = taxi.get_state()[0]
        x, y = obs[0], obs[1]
        if drop[0] == -1:
            tx, ty = stations[check]
        else:
            tx, ty = drop

        if np.random.rand() < eps:
            r = np.random.randint(0, 10)
            if r == 0:
                action = 1 if s[0]-5 < 0 else 0
            elif r == 1:
                action = 3 if s[1]-5 < 0 else 2
            else:
                action = np.random.randint(0, 6)
        else:
            action = np.argmax(
                q_table[s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8]])
        nobs, reward, done = taxi.step(np.array([action]))
        nobs = nobs[0]
        ncheck = check
        nstage = stage
        reward = reward[0]
        if stage == 0:
            if (x, y) == (tx, ty):
                if obs[14] == 1:
                    if action == 4:
                        nstage = 1
                        ncheck = 0
                        reward += 5
                        drop = (-1, -1)
                    else:
                        reward -= 1
                else:
                    ncheck = (check+1) % 4
                    reward += 0.1
        else:
            if (x, y) == stations[check]:
                if obs[15] == 1:
                    if action == 5:
                        reward += 10
                    else:
                        reward -= 1
                else:
                    ncheck = (check+1) % 4
                    reward += 0.1
            if action == 5:
                reward -= 7
                drop = (nobs[0], nobs[1])
                nstage = 0
        # print(f'({obs[0]}, {obs[1]}) ({tx}, {ty})')
        # print(f'Action {get_action_name(action)} {action} {
        #      check=} {ncheck=} {stage=} {nstage=}')
        # print(obs)
        # taxi.render(0)
        # input('')
        nx, ny = nobs[0], nobs[1]
        if drop[0] == -1:
            ntx, nty = stations[ncheck]
        else:
            ntx, nty = drop

        ns = [ntx-nx+9, nty-ny+9, nstage,
              nobs[10], nobs[11], nobs[12], nobs[13], nobs[14], nobs[15]]
        tot_reward += reward

        q_table[s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], action] += \
            alpha * (reward + gamma * np.max(
                q_table[ns[0], ns[1], ns[2], ns[3], ns[4], ns[5], ns[6], ns[7], ns[8]]) -
                q_table[s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8], action])

        stage = nstage
        check = ncheck
        s = ns
        if done:
            break
    if eps > 0.1:
        eps *= eps_rate1
    else:
        eps *= eps_rate2
    reward_hist.append(tot_reward)
    if epoch % 1000 == 0:
        print(f'Epoch : {epoch}  Avg Reward : {
              sum(reward_hist)/len(reward_hist):.5f}  Eps : {eps:.5f}')
        reward_hist = []
q_table.tofile('q_table.npz')
