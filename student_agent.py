import numpy as np

q_table = np.fromfile('q_table.npz').reshape((19, 19, 2, 2, 2, 2, 2, 2, 2, 6))

check = 0
stage = 0
drop = (-1, -1)


def get_action(obs):
    global check, stage, drop

    stations = [(obs[2+i*2], obs[2+i*2+1]) for i in range(4)]
    x, y = obs[0], obs[1]
    if drop[0] == -1:
        tx, ty = stations[check]
    else:
        tx, ty = drop

    s = [tx-x+9, ty-y+9, stage, obs[10], obs[11],
         obs[12], obs[13], obs[14], obs[15]]

    if np.random.rand() < 0.1:
        action = np.random.randint(0, 4)
    else:
        action = np.argmax(
            q_table[s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8]])
    if stage == 0:
        if (x, y) == (tx, ty):
            if obs[14] == 1:
                if action == 4:
                    stage = 1
                    check = 0
                    drop = (-1, -1)
            else:
                check = (check+1) % 4
    else:
        if (x, y) == stations[check]:
            if obs[15] != 1:
                check = (check+1) % 4
        if action == 5:
            drop = (obs[0], obs[1])
            stage = 0

    return action
