<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import Spinner from '$lib/components/Spinner.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "$lib/components/toasts/notifications";
    import StopButton from './StopButton.svelte';
    import Button from './Button.svelte';
    import Checkbox from './Checkbox.svelte';
    import type { SpinRemote } from '$lib/types/models';

    export let restPath: string;
    export let remotesPath: string = '/rest/spin-seq/remotes';

    interface SequencerState {
        status: {
            isRunning: boolean;
            selectedCommand: number;
            commandRunning: boolean;
            nextCommand: number;
            lostCommands: number;
        };
        config: {
            commands: Object[];
            selectedCommand: number;
            isRunning: boolean;
        };
    }

    let sequencerState: SequencerState;
    let remotes: SpinRemote[] = [];
    let remoteStates: {[key: string]: SequencerState} = {};
    let commandError = '';
    let editingCommand: {index: number, hostname: string} | null = null;
    let commandInput = '';

    let intervalID: any;
    onMount(() => {
        getRemotes();
        intervalID = setInterval(() => {
            updateAllStates();
        }, 1000);
    });
    onDestroy(() => {
        clearInterval(intervalID);
    });

    async function getRemotes() {
        return getJsonRest(remotesPath, { remotes }).then((data) => {
            remotes = data.remotes;
            updateAllStates();
        });
    }

    async function updateAllStates() {
        // Get master state
        await getJsonRest(restPath, sequencerState).then((data) => {
            sequencerState = data;
        });

        // Get all remote states
        for (let remote of remotes) {
            if (remote.ip) {
                try {
                    const remoteState = await getJsonRest(`http://${remote.ip}${restPath}`, null);
                    remoteStates[remote.hostname] = remoteState;
                } catch (error) {
                    console.error(`Failed to get state from ${remote.hostname}: ${error}`);
                }
            }
        }
    }

    function executeCommand(command: Object, hostname: string = 'master') {
        const path = hostname === 'master' ? restPath : `http://${remotes.find(r => r.hostname === hostname)?.ip}${restPath}`;
        postJsonRest(path + '/control', { 
            execute: JSON.stringify(command)
        }).then((data) => {
            if (data.execute?.error) {
                commandError = data.execute.error;
                notifications.error(`Error on ${hostname}: ${commandError}`, 3000);
            } else {
                notifications.success(`Command executed on ${hostname}`, 3000);
            }
        }).catch(error => {
            notifications.error(`Failed to execute on ${hostname}: ${error}`, 3000);
        });
    }

    function selectCommand(index: number, hostname: string = 'master') {
        const path = hostname === 'master' ? restPath : `http://${remotes.find(r => r.hostname === hostname)?.ip}${restPath}`;
        return postJsonRest(path + '/control', { select: index });
    }

    function stopAll() {
        // Stop master
        postJsonRest(restPath + '/control', { run: false });
        
        // Stop all remotes
        for (let remote of remotes) {
            if (remote.ip) {
                postJsonRest(`http://${remote.ip}${restPath}/control`, { run: false })
                    .catch(error => console.error(`Failed to stop ${remote.hostname}: ${error}`));
            }
        }
    }

    function executeCommandOnAll(index: number) {
        // Execute on master
        selectCommand(index, 'master');
        executeCommand(sequencerState.config.commands[index], 'master');

        // Execute on all remotes
        for (let remote of remotes) {
            if (remoteStates[remote.hostname]) {
                selectCommand(index, remote.hostname);
                executeCommand(remoteStates[remote.hostname].config.commands[index], remote.hostname);
            }
        }
    }

    function stringifyCommand(command: Object) {
        return JSON.stringify(command, null, 2);
    }

    function updateCommand(index: number, hostname: string, commandStr: string) {
        try {
            const command = JSON.parse(commandStr);
            const state = hostname === 'master' ? sequencerState : remoteStates[hostname];
            if (state) {
                state.config.commands[index] = command;
                const path = hostname === 'master' ? restPath : `http://${remotes.find(r => r.hostname === hostname)?.ip}${restPath}`;
                postJsonRest(path + '/config', state.config)
                    .then(() => {
                        notifications.success(`Command updated on ${hostname}`, 3000);
                        editingCommand = null;
                    })
                    .catch(error => {
                        notifications.error(`Failed to update command on ${hostname}: ${error}`, 3000);
                    });
            }
        } catch (err) {
            notifications.error("Invalid JSON format", 3000);
        }
    }
</script>

    {#if sequencerState && Object.keys(remoteStates).length > 0}
        <div class="overflow-x-auto w-full mb-4">
            <table class="w-full border-collapse min-w-[800px] table-fixed">
                <thead>
                    <tr class="bg-gray-100 dark:bg-gray-800">
                        <th class="p-2 text-left sticky left-0 bg-gray-100 dark:bg-gray-800 z-10 size-auto">ID</th>
                        <th class="p-2 text-left font-semibold size-auto">Master</th>
                        {#each remotes as remote}
                            {#if remoteStates[remote.hostname]}
                                <th class="p-2 text-left size-auto">
                                    <a href="http://{remote.ip}" target="_blank" rel="noopener noreferrer" 
                                       class="text-blue-500 hover:underline">
                                        {remote.hostname}
                                    </a>
                                    <div class="text-xs {remoteStates[remote.hostname].status.isRunning ? 'text-green-500' : 'text-red-500'}">
                                        {remoteStates[remote.hostname].status.isRunning ? 'Running' : 'Stopped'}
                                    </div>
                                </th>
                            {/if}
                        {/each}
                        <th class="p-2 text-center size-auto sticky right-0 bg-gray-100 dark:bg-gray-800">Actions</th>
                    </tr>
                </thead>
                <tbody>
                    {#each sequencerState.config.commands as _, index}
                        <tr class="border-t border-gray-300 dark:border-gray-700">
                            <td class="p-2 sticky left-0 bg-white dark:bg-gray-900 font-medium">
                                {index}
                            </td>
                            <td class="p-2">
                                <textarea
                                    class="w-full h-32 font-mono text-sm p-2 border rounded"
                                    value={stringifyCommand(sequencerState.config.commands[index])}
                                    on:change={(e) => updateCommand(index, 'master', e.target.value)}
                                />
                            </td>
                            {#each remotes as remote}
                                {#if remoteStates[remote.hostname]}
                                    <td class="p-2">
                                        <textarea
                                            class="w-full h-32 font-mono text-sm p-2 border rounded"
                                            value={stringifyCommand(remoteStates[remote.hostname].config.commands[index])}
                                            on:change={(e) => updateCommand(index, remote.hostname, e.target.value)}
                                        />
                                    </td>
                                {/if}
                            {/each}
                            <td class="p-2 text-center sticky right-0 bg-white dark:bg-gray-900">
                                <button
                                    class="w-full px-3 py-2 text-sm rounded bg-blue-500 hover:bg-blue-600 text-white"
                                    on:click={() => executeCommandOnAll(index)}
                                >
                                    Run
                                </button>
                            </td>
                        </tr>
                    {/each}
                </tbody>
            </table>
        </div>
        <div class="flex justify-between mt-4">
            <Button onClick={() => {
                sequencerState.config.commands.push({});
                postJsonRest(restPath + '/config', sequencerState.config);
            }} label="Add Command" />
            <StopButton onClick={stopAll} />
        </div>
    {:else}
        <Spinner />
    {/if}

<style>
    :global(.active) {
        color: var(--success);
        font-weight: bold;
    }
</style>