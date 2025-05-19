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
	import Collapsible from './Collapsible.svelte';

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
        getRemotes().then(updateAllStates);
        intervalID = setInterval(() => {
            // updateAllStates();
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
                    console.log(remoteState)
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

    function getMaxCommandCount(): number {
        return Math.max(
            ...Object.values(remoteStates).map(state => state.config.commands.length)
        );
    }

    function addCommandToAll() {
        // Add empty command to all remotes
        for (let remote of remotes) {
            if (remoteStates[remote.hostname]) {
                const state = remoteStates[remote.hostname];
                state.config.commands.push({});
                const path = `http://${remote.ip}${restPath}`;
                postJsonRest(path + '/config', state.config).catch(error => {
                    notifications.error(`Failed to add command to ${remote.hostname}: ${error}`, 3000);
                });
            }
        }
    }

    function getColumnWidth(hostname: string): string {
        const commands = remoteStates[hostname].config.commands;
        const maxLength = Math.max(...commands.map(cmd => JSON.stringify(cmd, null, 2).length));
        // Calculate width based on character count (approximating character width)
        return `${Math.max(Math.min(maxLength * 0.5, 300), 150)}px`; // min 150px, max 300px
    }
</script>
<Collapsible open>
    <span slot="title">Remotes Commands Grid</span>
    {#if sequencerState && Object.keys(remoteStates).length > 0}
        <div class="overflow-x-auto w-full mb-4">
            <div class="grid w-full" style="grid-template-columns: 4rem repeat({Object.keys(remoteStates).length}, minmax(150px, 1fr)) 6rem;">
                <div class="p-2 font-semibold bg-gray-100 dark:bg-gray-800">ID</div>
                {#each remotes as remote}
                    {#if remoteStates[remote.hostname]}
                        <div class="p-2 bg-gray-100 dark:bg-gray-800" style="min-width: {getColumnWidth(remote.hostname)}">
                            <a href="http://{remote.ip}" target="_blank" rel="noopener noreferrer" 
                               class="text-blue-500 hover:underline">
                                {remote.hostname}
                            </a>
                            <div class="text-xs {remoteStates[remote.hostname].status.isRunning ? 'text-green-500' : 'text-red-500'}">
                                {remoteStates[remote.hostname].status.isRunning ? 'Running' : 'Stopped'}
                            </div>
                        </div>
                    {/if}
                {/each}
                <div class="p-2 text-center font-semibold bg-gray-100 dark:bg-gray-800">Actions</div>

                {#each Array(getMaxCommandCount()) as _, index}
                    <div class="p-2 font-medium border-t border-gray-300 dark:border-gray-700">
                        {index}
                    </div>
                    {#each remotes as remote}
                        {#if remoteStates[remote.hostname]}
                            <div class="p-2 border-t border-gray-300 dark:border-gray-700" style="min-width: {getColumnWidth(remote.hostname)}">
                                <textarea
                                    class="w-full h-32 font-mono text-sm p-2 border rounded"
                                    value={remoteStates[remote.hostname].config.commands[index] ? 
                                        stringifyCommand(remoteStates[remote.hostname].config.commands[index]) : 
                                        '{}'}
                                    on:change={(e) => updateCommand(index, remote.hostname, e.target.value)}
                                />
                            </div>
                        {/if}
                    {/each}
                    <div class="p-2 text-center border-t border-gray-300 dark:border-gray-700">
                        <button
                            class="w-full px-3 py-2 text-sm rounded bg-blue-500 hover:bg-blue-600 text-white"
                            on:click={() => executeCommandOnAll(index)}
                        >
                            Run
                        </button>
                    </div>
                {/each}
            </div>
        </div>
        <div class="flex justify-between mt-4">
            <Button onClick={addCommandToAll} label="Add Command" />
            <StopButton onClick={stopAll} />
        </div>
    {:else}
        <Spinner />
    {/if}
</Collapsible>
<style>
    :global(.active) {
        color: var(--success);
        font-weight: bold;
    }
</style>