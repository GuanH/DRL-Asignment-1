import numpy as np

q_table = np.fromfile('q_table.npz').reshape((3, 3, 2, 2, 2, 2, 2, 2, 2, 6))

check = 0
stage = 0
drop = (-1, -1)


def get_action_name(action):
    """Returns a human-readable action name."""
    actions = ["Move South", "Move North", "Move East",
               "Move West", "Pick Up", "Drop Off"]
    return actions[action] if action is not None else "None"


def sign(x):
    return 1 if x > 0 else 2 if x < 0 else 0


def get_action(obs):
    global check, stage, drop, last_stage, last_check, last_action

    stations = [(obs[2+i*2], obs[2+i*2+1]) for i in range(4)]
    x, y = obs[0], obs[1]
    if drop[0] == -1:
        tx, ty = stations[check]
    else:
        tx, ty = drop

    s = [sign(tx-x), sign(ty-y), stage, obs[10], obs[11],
         obs[12], obs[13], obs[14], obs[15]]

    # if np.random.rand() < 0.1:
    #    v = np.random.rand(6)
    #    v[4] = -10000
    #    v[5] = -10000
    # else:
    v = q_table[s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[8]].copy()
    # 0 down 1 up 2 right 3 left
    if obs[10]:  # north
        v[1] = -10000
    if obs[11]:  # south
        v[0] = -10001
    if obs[12]:  # east
        v[2] = -10002
    if obs[13]:  # west
        v[3] = -10003
    if stage == 1 or obs[14] != 1 or (s[0], s[1]) != (0, 0):
        v[4] = -10004
    if stage == 0 or obs[15] != 1 or (s[0], s[1]) != (0, 0):
        v[5] = -10005
    action = np.argmax(v)
    last_check = check
    last_stage = stage
    last_action = action

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
