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
	import DisableButton from './DisableButton.svelte';
    

    export let restPath: string;
    export let remotesPath: string = '/rest/spin-seq/remotes';

    interface SequencerState {
        status?: {
            isRunning: boolean;
            selectedCommand: number;
            commandRunning: boolean;
            nextCommand: number;
            lostCommands: number;
        };
        config?: {
            commands: Object[];
            selectedCommand: number;
            isRunning: boolean;
        };
    }

    interface RemoteState extends SequencerState {
        hostname: string;
        ip: string;
        // remote: SpinRemote
    }

    let remotes: SpinRemote[] = [];
    let remoteStates: RemoteState[] = [];
    let commandError = '';
    let editingCommand: {index: number, hostname: string} | null = null;


    let intervalID: any;
    onMount(() => {
        getRemotes();
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

    async function updateRemoteState(remote: RemoteState, index: number) {
        return getJsonRest(`http://${remote.ip}${restPath}`, {}, {signal: AbortSignal.timeout(2000)})
        .then((data)=>{
            remoteStates[index] = {
                ...data,
                hostname: remote.hostname,
                ip: remote.ip
            }
            updateCombinedCommandsJson();
        }).catch((error)=>{
            remoteStates[index].hostname = remote.hostname;
            notifications.error(`Failed to get state from ${remote.hostname}: ${error}`, 2000);
        })
    }

    async function updateAllStates() {
        // Get all remote states
        while (remoteStates.length < remotes.length) remoteStates.push({hostname: "", ip: ""});
        // const newStates: RemoteState[] = remotes.map((r)=> {return {hostname: r.hostname, ip: r.ip}});
        for (let [index, remote] of remotes.entries()) {
            if (remote.ip) {
                updateRemoteState(remote, index);
            }
        }
        // remoteStates = newStates;
    }

    async function executeCommand(command: Object, hostname: string = 'master') {
        const path = hostname === 'master' ? restPath : `http://${remotes.find(r => r.hostname === hostname)?.ip}${restPath}`;
        return postJsonRest(path + '/control', { 
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
        // Stop all remotes
        for (let remote of remotes) {
            if (remote.ip) {
                postJsonRest(`http://${remote.ip}${restPath}/control`, { run: false })
                    .catch(error => console.error(`Failed to stop ${remote.hostname}: ${error}`));
            }
        }
    }

    function startAll() {
        // Start all remotes
        for (let remote of remotes) {
            if (remote.ip) {
                postJsonRest(`http://${remote.ip}${restPath}/control`, { run: true })
                    .catch(error => console.error(`Failed to start ${remote.hostname}: ${error}`));
            }
        }
    }

    function executeCommandOnAll(index: number) {
        // Execute on all remotes
        for (let state of remoteStates) {
            if (state.config) {
                selectCommand(index, state.hostname);
                executeCommand(state.config.commands[index], state.hostname);
            }
        }
        updateAllStates();
    }

    function selectCommandOnAll(index: number) {
        // Execute on all remotes
        for (let state of remoteStates) {
            if (state.config) {
                selectCommand(index, state.hostname);
            }
        }
        updateAllStates();
    }

    function deleteCommandOnAll(index: number) {
        // Execute on all remotes
        for (let state of remoteStates) {
            if (state.config) {
                delete state.config.commands[index];
                postJsonRest(`http://${state.ip}${restPath}/config`, state.config).catch(error => {
                    notifications.error(`Failed to delete on ${state.hostname}: ${error}`, 3000);
                });
            }
        }
        updateAllStates();
    }

    function stringifyCommand(command: Object) {
        return JSON.stringify(command, null, 2);
    }

    function updateCommand(index: number, state: RemoteState, commandStr: string) {
        try {
            const command = eval('('+commandStr+')');
            if (state?.config) {
                state.config.commands[index] = command;
                const path = `http://${state.ip}${restPath}`;
                postJsonRest(path + '/config', state.config)
                    .then(() => {
                        notifications.success(`Command updated on ${state.hostname}`, 3000);
                        editingCommand = null;
                    })
                    .catch(error => {
                        notifications.error(`Failed to update command on ${state.hostname}: ${error}`, 3000);
                    });
            }
        } catch (err) {
            notifications.error("Invalid JSON format", 3000);
        }
    }

    function getMaxCommandCount(): number {
        return Math.max(
            ...remoteStates.map(state => state.config?.commands.length || 0)
        );
    }

    function addCommandToAll() {
        // Add empty command to all remotes
        let maxCommands = getMaxCommandCount();
        for (let state of remoteStates) {
            if (state.config) {
                while (state.config.commands.length < maxCommands + 1) state.config.commands.push({});
                postJsonRest(`http://${state.ip}${restPath}/config`, state.config).catch(error => {
                    notifications.error(`Failed to add command to ${state.hostname}: ${error}`, 3000);
                });
            }
        }
        updateAllStates();
    }

    let combinedCommandsJson = "";

    function updateCombinedCommandsJson() {
        if (remoteStates.length > 0) {
            const commands = {};
            remoteStates.forEach(state => {
                if (state.config && state.hostname) {
                    commands[state.hostname] = state.config.commands;
                }
            });
            combinedCommandsJson = JSON.stringify(commands, null, 2);
        }
    }

    async function updateAllFromJson() {
        try {
            const commands = JSON.parse(combinedCommandsJson);
            console.log("Parsed commands: ", commands);
            for (let state of remoteStates) {
                if (state.config && state.hostname && commands[state.hostname]) {
                    state.config.commands = commands[state.hostname];
                    console.log("Updating commands for ", state.hostname, state.config.commands);
                    postJsonRest(`http://${state.ip}${restPath}/config`, state.config).catch(error => {
                        notifications.error(`Failed to update ${state.hostname}: ${error}`, 3000);
                    });
                }
            }
            notifications.success("Commands updated on all remotes", 3000);
        } catch (error) {
            notifications.error("Invalid JSON format", 3000);
        }
    }

    async function disableAllMotors() {
        return postJsonRest("/rest/espnow", {broadcast: "{p:{en:0,seq:0}}"});
    }

    async function stopAllMotors() {
        return postJsonRest("/rest/espnow", {broadcast: "{c:\"stop\",p:{seq:0}}"});
    }

    async function triggerSequenceOnAll(sequence: number) {
        return postJsonRest("/rest/espnow", {broadcast: "{t:" + String(sequence) + "}"});
    }

</script>
<!-- <Collapsible open>
    <span slot="title">Remotes Commands Grid</span> -->

        <div class="flex flex-row justify-between items-center m-2">
            <Button onClick={()=>triggerSequenceOnAll(0)} label="Init" />
            <Button onClick={()=>triggerSequenceOnAll(1)} label="Start" />
            <StopButton onClick={stopAllMotors} />
            <DisableButton onClick={disableAllMotors}/>
        </div>

    {#if remoteStates.length > 0}
        
        <div class="overflow-x-auto w-full mb-4">
            <div class="grid w-full" style="grid-template-columns: 4rem repeat({remoteStates.length}, minmax(180px, 1fr)) 6rem;">
                <div class="p-2 font-semibold bg-gray-100 dark:bg-gray-800">ID</div>
                {#each remoteStates as state}
                    <div class="p-2 bg-gray-100 dark:bg-gray-800 min-w-fit">
                        <a href="http://{state.ip}" target="_blank" rel="noopener noreferrer" 
                           class="text-blue-500 hover:underline">
                            {state.hostname}
                        </a>
                        {#if state.status}
                        <div class="text-xs {state.status.isRunning ? 'text-green-500' : 'text-red-500'}">
                            {state.status.isRunning ? 'Running' : 'Stopped'}
                        </div>
                        {/if}
                    </div>
                {/each}
                <div class="p-2 text-center font-semibold bg-gray-100 dark:bg-gray-800">Actions</div>

                {#each Array(getMaxCommandCount()) as _, index}
                    <div class="p-2 font-medium border-t border-gray-300 dark:border-gray-700">
                        {index}
                    </div>
                    {#each remoteStates as state}
                        <div class="p-2 border-t border-gray-300 dark:border-gray-700 min-w-fit"
                             class:bg-blue-100={state.status?.selectedCommand === index}
                             class:dark:bg-blue-900={state.status?.selectedCommand === index}>
                             {#if state.config}
                            <textarea
                                class="w-full h-32 font-mono text-sm p-2 border"
                                value={state.config.commands[index] ? 
                                    stringifyCommand(state.config.commands[index]) : 
                                    '{}'}
                                on:change={(e) => updateCommand(index, state, e.target.value)}
                            />
                            {/if}
                        </div>
                    {/each}
                    <div class="p-2 text-center border-t border-gray-300 dark:border-gray-700">
                        <button
                            class="w-full px-3 py-2 text-sm rounded bg-blue-500 hover:bg-blue-600 text-white mb-2"
                            on:click={() => executeCommandOnAll(index)}
                        >
                            Run
                        </button>
                        <button
                            class="w-full px-3 py-2 text-sm rounded bg-blue-500 hover:bg-blue-600 text-white mb-2"
                            on:click={() => selectCommandOnAll(index)}
                        >
                            Select
                        </button>
                        <!-- <button
                            class="w-full px-3 py-2 text-sm rounded bg-red-500 hover:bg-red-600 text-white mb-2"
                            on:click={() => deleteCommandOnAll(index)}
                        >
                            Delete
                        </button> -->
                    </div>
                {/each}
            </div>
        </div>
        <div class="mx-0 my-1 flex flex-col space-y-4 sm:mx-8 sm:my-8 justify-between mt-4">
            <Button onClick={addCommandToAll} label="Add Command" />
            <Button onClick={startAll} label="Start" />
            <StopButton onClick={stopAll} />
        </div>

        <div class="mb-4">
            <div class="flex justify-between items-center mb-2">
                <h3 class="text-lg font-semibold">All Commands</h3>
                <Button onClick={updateAllFromJson} label="Save All Commands" />
            </div>
            <textarea
                class="w-full h-96 font-mono text-sm p-2 border rounded"
                bind:value={combinedCommandsJson}
                placeholder="Edit commands for all remotes in JSON format"
            />
        </div>
    {:else}
        <Spinner />
    {/if}
<!-- </Collapsible> -->
<style>
    :global(.active) {
        color: var(--success);
        font-weight: bold;
    }
</style>