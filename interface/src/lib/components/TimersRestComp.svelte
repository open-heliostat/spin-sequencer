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
	import Checkbox from './Checkbox.svelte';

    export let restPath: string;

    interface Timer {
        hour: number;
        minute: number;
        days: number;
        command: string;
        executed: boolean;
    }

    interface TimersState {
        timers: Timer[];
        executeLatestOnStart: boolean;
    }

    const DAYS = [
        { value: 0x01, label: 'Sun' },
        { value: 0x02, label: 'Mon' },
        { value: 0x04, label: 'Tue' },
        { value: 0x08, label: 'Wed' },
        { value: 0x10, label: 'Thu' },
        { value: 0x20, label: 'Fri' },
        { value: 0x40, label: 'Sat' }
    ];

    let timersState: TimersState = {
        timers: [],
        executeLatestOnStart: false
    };

    let newTimer = {
        hour: 0,
        minute: 0,
        days: 0x7F, // All days by default
        command: ""
    };

    function getDayString(days: number): string {
        if (days === 0x7F) return 'Every day';
        if (days === 0x3E) return 'Weekdays';
        if (days === 0x41) return 'Weekends';
        return DAYS.filter(d => days & d.value)
                  .map(d => d.label)
                  .join(', ');
    }

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
                days: newTimer.days,
                command: newTimer.command
            }
        }).then(() => {
            notifications.success("Timer added", 3000);
            newTimer.command = "";
            getTimers();
        });
    }

    function removeTimer(hour: number, minute: number, days: number) {
        postJsonRest(restPath, {
            remove: {
                hour: hour,
                minute: minute,
                days: days
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
    {#snippet icon()}
        <Clock class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
    {/snippet}
    {#snippet title()}
        <span>Daily Timers</span>
    {/snippet}

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
                    <div class="flex flex-col w-48"> <!-- Added fixed width -->
                        <label class="label">
                            <span class="label-text">Days</span>
                        </label>
                        <div class="grid grid-cols-4 gap-1"> <!-- Changed to grid layout -->
                            {#each DAYS as day}
                                <label class="label cursor-pointer justify-start p-1">
                                    <input 
                                        type="checkbox" 
                                        class="checkbox checkbox-xs"
                                        checked={newTimer.days & day.value}
                                        on:change={(e) => {
                                            if (e.target.checked) {
                                                newTimer.days |= day.value;
                                            } else {
                                                newTimer.days &= ~day.value;
                                            }
                                        }}
                                    />
                                    <span class="label-text text-sm ml-1">{day.label}</span> <!-- Made text smaller -->
                                </label>
                            {/each}
                        </div>
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
                </div>
        </div>

        <div class="w-full flex-row flex">
            <div class="flex-grow flex"></div>
            <Button label="Add Timer" onClick={addTimer} />
        </div>

        <div class="overflow-x-auto">
            <table class="table w-full">
                <thead>
                    <tr>
                        <th>Time</th>
                        <th>Days</th>
                        <th class="w-full">Command</th>
                        <th>Status</th>
                        <th>Actions</th>
                    </tr>
                </thead>
                <tbody>
                    {#await getTimers()}
                        <tr>
                            <td colspan="5" class="text-center">
                                <Spinner />
                            </td>
                        </tr>
                    {:then data}
                        {#if timersState.timers.length === 0}
                            <tr>
                                <td colspan="5" class="text-center">No timers configured</td>
                            </tr>
                        {:else}
                            {#each timersState.timers as timer}
                                <tr>
                                    <td>{timer.hour.toString().padStart(2, '0')}:{timer.minute.toString().padStart(2, '0')}</td>
                                    <td>{getDayString(timer.days)}</td>
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
                                            on:click={() => removeTimer(timer.hour, timer.minute, timer.days)}
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

        <GridForm>
            <Checkbox
                label="Execute latest timer on start"
                bind:value={timersState.executeLatestOnStart}
                onChange={() => {
                    postJsonRest(restPath, { executeLatestOnStart: timersState.executeLatestOnStart });
                }}
            />
        </GridForm>
    </div>
</SettingsCard>