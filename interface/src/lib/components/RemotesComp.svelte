<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from './GridForm.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "./toasts/notifications";
    import Button from './Button.svelte';
    import Text from './Text.svelte';
    import type { SpinRemote } from '$lib/types/models';
    import Remote from '~icons/tabler/network';
    import Spinner from './Spinner.svelte';
	import Slider from './Slider.svelte';
	import RemoteComp from './RemoteComp.svelte';

    export let restPath: string;
    
    let remotes: SpinRemote[] = [];
    let newRemote = {
        hostname: '',
        ip: '',
        rxId: 99
    };

    async function getRemotes() {
        return getJsonRest(restPath, { remotes }).then((data) => {
            remotes = data.remotes;
            console.log("Remotes: ", data);
        });
    }

    async function postRemotes() {
        return postJsonRest(restPath, { remotes }).then((data) => {
            notifications.success(`Updated remotes`, 3000);
            console.log("Remotes: ", data);
            return data;
        }).catch((error) => {
            notifications.error(`Failed to update remotes: ${error}`, 3000);
        });
    }

    async function addRemote() {
        if (newRemote.ip || newRemote.rxId) {
            return postJsonRest(restPath, { 
                addRemote: newRemote 
            }).then(() => {
                notifications.success(`Added remote ${newRemote.hostname}`, 3000);
                newRemote = { hostname: '', ip: '', rxId: 99 };
                return getRemotes();
            }).catch((error) => {
                notifications.error(`Failed to add remote: ${error}`, 3000);
            });
        }
    }

    async function removeRemote(index: number) {
        return postJsonRest(restPath, {
            removeRemote: {
                index: index
            }
        }).then(() => {
            notifications.success(`Removed remote ${remotes[index].hostname}`, 3000);
            return getRemotes();
        }).catch((error) => {
            notifications.error(`Failed to remove remote: ${error}`, 3000);
        });
    }

    onMount(() => {
        getRemotes();
    });
</script>

<SettingsCard>
    <Remote slot="icon" class="flex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">Remotes</span>

    <!-- Add new remote form -->
    <div class="mb-4">
        <h3 class="text-lg font-semibold mb-2">Add New Remote</h3>
        <GridForm>
            <Text
                label="Hostname"
                bind:value={newRemote.hostname}
            />
            <Text
                label="IP Address"
                bind:value={newRemote.ip}
            />
            <Slider
                label="RX ID"
                bind:value={newRemote.rxId}
            />
        </GridForm>
        <div class="flex flex-row flex-wrap justify-between gap-x-2">
            <div class="flex-grow"></div>
            <div>
                <div>
                    <Button
                        onClick={addRemote}
                        label="Add Remote"
                    />
                </div>
            </div>
        </div>
    </div>

    <!-- Remotes list -->
    <div>
        <h3 class="text-lg font-semibold mb-2">Connected Remotes</h3>
        {#if remotes.length === 0}
            <p class="text-gray-500 dark:text-gray-400">No remotes configured</p>
        {:else}
            <div class="grid gap-4">
                {#each remotes as remote, index}
                    <div class="bg-base-200 p-4 rounded-lg flex items-center justify-between">
                        <div>
                            <p class="font-semibold">{remote.hostname}</p>
                            <p class="text-sm text-gray-600 dark:text-gray-400">
                                IP: {remote.ip || 'Not Connected'} | RX ID: {remote.rxId}
                            </p>
                        </div>
                        <Button
                            onClick={() => removeRemote(index)}
                            label="Remove"
                        />
                    </div>
                {/each}
            </div>
        {/if}
    </div>
</SettingsCard>

{#each remotes as remote}
    <RemoteComp
        bind:remote={remote}
        onChange={postRemotes}
    />
{/each}

<style>
    .grid {
        display: grid;
        gap: 1rem;
    }
</style>