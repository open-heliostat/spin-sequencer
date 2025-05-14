<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from './GridForm.svelte';
    import Spinner from './Spinner.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "./toasts/notifications";
    import Button from './Button.svelte';
    import Text from './Text.svelte';
    import Clock from '~icons/tabler/clock';

    export let restPath: string;

    interface Timer {
        hour: number;
        minute: number;
        command: string;
        executed: boolean;
    }

    interface TimersState {
        timers: Timer[];
    }

    let timersState: TimersState = {
        timers: []
    };

    let newTimer = {
        hour: 0,
        minute: 0,
        command: ""
    };

    async function getTimers() {
        return getJsonRest(restPath, timersState).then((data) => {
            timersState = data;
            return timersState;
        });
    }

    function addTimer() {
        if (newTimer.command.trim() === "") {
            notifications.error("Command cannot be empty", 3000);
            return;
        }
        
        postJsonRest(restPath, {
            add: {
                hour: newTimer.hour,
                minute: newTimer.minute,
                command: newTimer.command
            }
        }).then(() => {
            notifications.success("Timer added", 3000);
            newTimer.command = "";
            getTimers();
        });
    }

    function removeTimer(hour: number, minute: number) {
        postJsonRest(restPath, {
            remove: {
                hour: hour,
                minute: minute
            }
        }).then(() => {
            notifications.success("Timer removed", 3000);
            getTimers();
        });
    }

    onMount(() => {
        getTimers();
    });
</script>

<SettingsCard>
    <Clock slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">Daily Timers</span>

    <div class="w-full overflow-x-auto">
        <div class="mb-4">
            <h3 class="text-lg font-semibold mb-2">Add New Timer</h3>

                <div class="flex gap-4 w-full">
                    <div class="flex flex-col">
                        <label class="label">
                            <span class="label-text">Hour</span>
                        </label>
                        <input 
                            type="number" 
                            class="input input-bordered w-full" 
                            min="0" 
                            max="23" 
                            bind:value={newTimer.hour}
                        />
                    </div>
                    <div class="flex flex-col">
                        <label class="label">
                            <span class="label-text">Minute</span>
                        </label>
                        <input 
                            type="number" 
                            class="input input-bordered w-full" 
                            min="0" 
                            max="59" 
                            bind:value={newTimer.minute}
                        />
                    </div>
                    <div class="flex-grow">
                        <label class="label">
                            <span class="label-text">Command</span>
                        </label>
                        <input 
                            type="text" 
                            class="input input-bordered w-full" 
                            placeholder="Enter command"
                            bind:value={newTimer.command}
                        />
                    </div>
                    <div class="self-end">
                        <Button label="Add Timer" onClick={addTimer} />
                    </div>
                </div>
        </div>

        <div class="overflow-x-auto">
            <table class="table w-full">
                <thead>
                    <tr>
                        <th>Time</th>
                        <th class="w-full">Command</th>
                        <th>Status</th>
                        <th>Actions</th>
                    </tr>
                </thead>
                <tbody>
                    {#await getTimers()}
                        <tr>
                            <td colspan="4" class="text-center">
                                <Spinner />
                            </td>
                        </tr>
                    {:then data}
                        {#if timersState.timers.length === 0}
                            <tr>
                                <td colspan="4" class="text-center">No timers configured</td>
                            </tr>
                        {:else}
                            {#each timersState.timers as timer}
                                <tr>
                                    <td>{timer.hour.toString().padStart(2, '0')}:{timer.minute.toString().padStart(2, '0')}</td>
                                    <td>{timer.command}</td>
                                    <td>
                                        {#if timer.executed}
                                            <span class="badge badge-success">Executed</span>
                                        {:else}
                                            <span class="badge">Pending</span>
                                        {/if}
                                    </td>
                                    <td>
                                        <button 
                                            class="btn btn-error btn-sm"
                                            on:click={() => removeTimer(timer.hour, timer.minute)}
                                        >
                                            Remove
                                        </button>
                                    </td>
                                </tr>
                            {/each}
                        {/if}
                    {/await}
                </tbody>
            </table>
        </div>
    </div>
</SettingsCard>